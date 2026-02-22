package deepseek.java;

import java.io.*;
import java.nio.file.*;
import java.time.*;
import java.time.format.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

public class cwe_460 {

    // Configuration
    private static final String LOG_DIR = "logs";
    private static final String APP_LOG = "application.log";
    private static final String ERROR_LOG = "error.log";
    private static final String AUDIT_LOG = "audit.log";

    // ==================== LOG ENTRY CLASS ====================

    static class LogEntry {
        private final String level;
        private final String message;
        private final String thread;
        private final LocalDateTime timestamp;
        private final Map<String, String> context;

        public LogEntry(String level, String message) {
            this.level = level;
            this.message = message;
            this.thread = Thread.currentThread().getName();
            this.timestamp = LocalDateTime.now();
            this.context = new HashMap<>();
        }

        public LogEntry withContext(String key, String value) {
            context.put(key, value);
            return this;
        }

        public String format() {
            StringBuilder sb = new StringBuilder();
            sb.append(timestamp.format(DateTimeFormatter.ISO_LOCAL_DATE_TIME));
            sb.append(" [").append(thread).append("] ");
            sb.append(level).append(": ").append(message);

            if (!context.isEmpty()) {
                sb.append(" {");
                context.forEach((k, v) -> sb.append(k).append("=").append(v).append(", "));
                sb.setLength(sb.length() - 2);
                sb.append("}");
            }

            return sb.toString();
        }
    }

    // ==================== INSECURE LOGGER (CWE-460 VULNERABLE)
    // ====================

    static class InsecureLogger {

        private FileWriter fileWriter;
        private final String logFile;
        private boolean closed = false;

        public InsecureLogger(String logFile) throws IOException {
            this.logFile = logFile;
            createLogDirectory();
            this.fileWriter = new FileWriter(logFile, true);
            System.out.println("[INSECURE] Logger opened: " + logFile);
        }

        private void createLogDirectory() {
            new File(LOG_DIR).mkdirs();
        }

        // INSECURE: No cleanup if exception occurs
        public void log(String message) throws IOException {
            if (closed) {
                throw new IOException("Logger already closed");
            }

            LogEntry entry = new LogEntry("INFO", message);
            String formatted = entry.format() + System.lineSeparator();

            // INSECURE: If this throws, fileWriter is not closed!
            fileWriter.write(formatted);
            fileWriter.flush();
        }

        // INSECURE: Exception in logError prevents cleanup
        public void logError(String message, Exception e) throws IOException {
            if (closed) {
                throw new IOException("Logger already closed");
            }

            LogEntry entry = new LogEntry("ERROR", message)
                    .withContext("exception", e.getClass().getName())
                    .withContext("message", e.getMessage());

            String formatted = entry.format() + System.lineSeparator();
            fileWriter.write(formatted);

            // INSECURE: This might throw, leaving fileWriter open
            e.printStackTrace(new PrintWriter(fileWriter, true));

            fileWriter.flush();
        }

        // INSECURE: No cleanup in this method at all
        public void unsafeWrite(String data) throws IOException {
            fileWriter.write(data); // If this throws, resource leak!
        }

        // INSECURE: Multiple exit paths without cleanup
        public void processWithMultipleExits(String data) throws IOException {
            if (data == null) {
                throw new IllegalArgumentException("Data cannot be null"); // No cleanup!
            }

            fileWriter.write(data);

            if (data.length() < 10) {
                throw new IOException("Data too short"); // No cleanup!
            }

            fileWriter.flush(); // Success path, but still no explicit close
        }

        public void close() {
            if (!closed) {
                try {
                    fileWriter.close();
                    closed = true;
                    System.out.println("[INSECURE] Logger closed: " + logFile);
                } catch (IOException e) {
                    System.err.println("[INSECURE] Error closing logger: " + e.getMessage());
                }
            }
        }
    }

    // ==================== INSECURE LOGGER WITH FINALIZE (BAD PRACTICE)
    // ====================

    static class InsecureLoggerWithFinalize {

        private FileWriter fileWriter;
        private final String logFile;

        public InsecureLoggerWithFinalize(String logFile) throws IOException {
            this.logFile = logFile;
            new File(LOG_DIR).mkdirs();
            this.fileWriter = new FileWriter(logFile, true);
        }

        public void log(String message) throws IOException {
            fileWriter.write(message + System.lineSeparator());
            fileWriter.flush();
        }

        // INSECURE: finalize is unpredictable and deprecated
        @Override
        protected void finalize() throws Throwable {
            try {
                if (fileWriter != null) {
                    fileWriter.close();
                    System.out.println("[INSECURE-FINALIZE] Closed in finalize");
                }
            } finally {
                super.finalize();
            }
        }
    }

    // ==================== SECURE LOGGER (CWE-460 MITIGATED) ====================

    static class SecureLogger implements AutoCloseable {

        private final FileWriter fileWriter;
        private final String logFile;
        private final Object lock = new Object();
        private boolean closed = false;
        private final AtomicInteger writeCount = new AtomicInteger(0);
        private final List<String> errorBuffer = new ArrayList<>();

        public SecureLogger(String logFile) throws IOException {
            this.logFile = logFile;
            createLogDirectory();

            // SECURE: Open in constructor, but with proper handling
            try {
                this.fileWriter = new FileWriter(logFile, true);
            } catch (IOException e) {
                throw new IOException("Failed to open log file: " + logFile, e);
            }

            // Write initialization marker
            writeInitMarker();
        }

        private void createLogDirectory() throws IOException {
            Path logPath = Paths.get(LOG_DIR);
            if (!Files.exists(logPath)) {
                try {
                    Files.createDirectories(logPath);
                } catch (IOException e) {
                    throw new IOException("Failed to create log directory", e);
                }
            }

            // Check permissions
            if (!Files.isWritable(logPath)) {
                throw new IOException("Log directory is not writable: " + logPath);
            }
        }

        private void writeInitMarker() {
            try {
                LogEntry entry = new LogEntry("INIT", "Logger initialized");
                synchronized (lock) {
                    fileWriter.write(entry.format() + System.lineSeparator());
                    fileWriter.flush();
                }
            } catch (IOException e) {
                System.err.println("[SECURE] Failed to write init marker: " + e.getMessage());
            }
        }

        // SECURE: Proper cleanup in all paths
        public void log(String message) throws IOException {
            if (closed) {
                throw new IOException("Logger is closed");
            }

            LogEntry entry = new LogEntry("INFO", message);

            synchronized (lock) {
                try {
                    fileWriter.write(entry.format() + System.lineSeparator());
                    fileWriter.flush();
                    writeCount.incrementAndGet();
                } catch (IOException e) {
                    // SECURE: Buffer error for later handling
                    bufferError("Failed to write log: " + e.getMessage());
                    throw new IOException("Logging failed", e);
                }
            }
        }

        // SECURE: Proper error logging with cleanup
        public void logError(String message, Exception e) throws IOException {
            if (closed) {
                throw new IOException("Logger is closed");
            }

            LogEntry entry = new LogEntry("ERROR", message)
                    .withContext("exception", e.getClass().getName())
                    .withContext("message", e.getMessage() != null ? e.getMessage() : "null");

            synchronized (lock) {
                try {
                    fileWriter.write(entry.format() + System.lineSeparator());

                    // Write stack trace
                    StringWriter sw = new StringWriter();
                    PrintWriter pw = new PrintWriter(sw);
                    e.printStackTrace(pw);
                    pw.flush();

                    for (String line : sw.toString().split(System.lineSeparator())) {
                        fileWriter.write("  " + line + System.lineSeparator());
                    }

                    fileWriter.flush();
                    writeCount.incrementAndGet();

                } catch (IOException ex) {
                    bufferError("Failed to write error log: " + ex.getMessage());
                    throw new IOException("Error logging failed", ex);
                }
            }
        }

        // SECURE: Safe write with proper handling
        public void safeWrite(String data) throws IOException {
            if (closed) {
                throw new IOException("Logger is closed");
            }

            synchronized (lock) {
                try {
                    fileWriter.write(data);
                    fileWriter.flush();
                } catch (IOException e) {
                    bufferError("Failed to write data: " + e.getMessage());
                    throw new IOException("Write failed", e);
                }
            }
        }

        // SECURE: Method with multiple exit points but proper cleanup
        public void processWithCleanup(String data) throws IOException {
            Objects.requireNonNull(data, "Data cannot be null");

            synchronized (lock) {
                try {
                    fileWriter.write(data);

                    if (data.length() < 10) {
                        throw new IOException("Data too short");
                    }

                    fileWriter.flush();

                } catch (IOException e) {
                    bufferError("Process failed: " + e.getMessage());
                    throw e; // Re-throw after buffering
                }
            }
        }

        // SECURE: Try-with-resources pattern
        public void writeBatch(List<String> messages) throws IOException {
            // SECURE: Use try-with-resources for any new resources
            File tempFile = File.createTempFile("logbatch", ".tmp");

            try (FileWriter tempWriter = new FileWriter(tempFile)) {
                for (String message : messages) {
                    tempWriter.write(message + System.lineSeparator());
                }
                tempWriter.flush();

                // SECURE: Now safely append to main log
                synchronized (lock) {
                    Files.write(Paths.get(logFile),
                            Files.readAllBytes(tempFile.toPath()),
                            StandardOpenOption.APPEND);
                }

            } finally {
                // SECURE: Always clean up temp file
                Files.deleteIfExists(tempFile.toPath());
            }
        }

        private void bufferError(String error) {
            synchronized (errorBuffer) {
                errorBuffer.add(LocalDateTime.now() + ": " + error);
                if (errorBuffer.size() > 100) {
                    errorBuffer.remove(0);
                }
            }
        }

        public List<String> getRecentErrors() {
            synchronized (errorBuffer) {
                return new ArrayList<>(errorBuffer);
            }
        }

        public int getWriteCount() {
            return writeCount.get();
        }

        public boolean isClosed() {
            return closed;
        }

        // SECURE: AutoCloseable implementation
        @Override
        public void close() {
            synchronized (lock) {
                if (closed) {
                    return;
                }

                try {
                    // Write closing marker
                    LogEntry entry = new LogEntry("SHUTDOWN", "Logger closing");
                    fileWriter.write(entry.format() + System.lineSeparator());
                    fileWriter.flush();

                } catch (IOException e) {
                    System.err.println("[SECURE] Failed to write closing marker: " + e.getMessage());
                } finally {
                    try {
                        fileWriter.close();
                    } catch (IOException e) {
                        System.err.println("[SECURE] Error closing file: " + e.getMessage());
                    } finally {
                        closed = true;
                        System.out.println("[SECURE] Logger closed: " + logFile);
                    }
                }
            }
        }
    }

    // ==================== SECURE LOGGER WITH BUFFER ====================

    static class BufferedSecureLogger implements AutoCloseable {

        private final FileWriter fileWriter;
        private final String logFile;
        private final List<String> buffer = new ArrayList<>();
        private final int bufferSize;
        private final Object lock = new Object();
        private boolean closed = false;
        private boolean autoFlush = true;

        public BufferedSecureLogger(String logFile, int bufferSize) throws IOException {
            this.logFile = logFile;
            this.bufferSize = bufferSize;
            new File(LOG_DIR).mkdirs();
            this.fileWriter = new FileWriter(logFile, true);
        }

        public void log(String message) throws IOException {
            synchronized (lock) {
                if (closed) {
                    throw new IOException("Logger is closed");
                }

                buffer.add(message);

                if (autoFlush && buffer.size() >= bufferSize) {
                    flush();
                }
            }
        }

        public void flush() throws IOException {
            synchronized (lock) {
                if (buffer.isEmpty()) {
                    return;
                }

                try {
                    for (String msg : buffer) {
                        fileWriter.write(msg + System.lineSeparator());
                    }
                    fileWriter.flush();
                    buffer.clear();
                } catch (IOException e) {
                    // SECURE: Keep buffer in case of error
                    throw new IOException("Failed to flush buffer", e);
                }
            }
        }

        @Override
        public void close() {
            synchronized (lock) {
                if (closed) {
                    return;
                }

                try {
                    flush(); // SECURE: Flush buffer before closing
                } catch (IOException e) {
                    System.err.println("[BUFFERED] Error flushing buffer: " + e.getMessage());
                } finally {
                    try {
                        fileWriter.close();
                    } catch (IOException e) {
                        System.err.println("[BUFFERED] Error closing file: " + e.getMessage());
                    } finally {
                        closed = true;
                    }
                }
            }
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class LoggerDemonstrator {

        public static void demonstrateInsecureLogging() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE LOGGING (CWE-460)");
            System.out.println("=".repeat(60));

            InsecureLogger logger = null;

            try {
                logger = new InsecureLogger(LOG_DIR + "/" + APP_LOG);

                // Normal logging
                logger.log("Application started");
                logger.log("User logged in: john_doe");

                // This will throw an exception
                System.out.println("\n🔴 Triggering exception...");
                logger.unsafeWrite(null); // This will throw NullPointerException

                // This line won't be reached
                logger.log("This won't be logged");

            } catch (Exception e) {
                System.err.println("[INSECURE] Caught exception: " + e.getMessage());
                // But logger is still open! Resource leak!
            }

            // Try to use logger after exception
            if (logger != null) {
                try {
                    // Logger might be in inconsistent state
                    logger.log("Trying to log after exception");
                } catch (Exception e) {
                    System.err.println("[INSECURE] Logger in bad state: " + e.getMessage());
                }
            }

            System.out.println("\n⚠️ PROBLEM: Logger may still be open (resource leak)");
            System.out.println("  File handle not closed due to exception");
        }

        public static void demonstrateInsecureFinalize() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE FINALIZE DEMONSTRATION");
            System.out.println("=".repeat(60));

            try {
                InsecureLoggerWithFinalize logger = new InsecureLoggerWithFinalize(
                        LOG_DIR + "/finalize.log");
                logger.log("Test message");

                // Not closing - relying on finalize (BAD!)
                logger = null;

                System.gc();
                System.out.println("GC triggered, but finalize may not run immediately");

            } catch (IOException e) {
                System.err.println("Error: " + e.getMessage());
            }

            System.out.println("\n⚠️ PROBLEM: finalize() is unpredictable and deprecated!");
        }

        public static void demonstrateSecureLogging() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE LOGGING (MITIGATED)");
            System.out.println("=".repeat(60));

            // SECURE: Using try-with-resources
            try (SecureLogger logger = new SecureLogger(LOG_DIR + "/" + APP_LOG)) {

                logger.log("Application started");
                logger.log("User logged in: john_doe");

                // This will throw but logger will be closed properly
                System.out.println("\n🟢 Attempting operation that will fail...");
                try {
                    logger.processWithCleanup("short");
                } catch (IOException e) {
                    System.out.println("  Caught expected exception: " + e.getMessage());
                    // Logger still usable because we handled it properly
                }

                // Continue logging
                logger.log("Continuing after handled exception");
                logger.logError("Test error", new RuntimeException("Test exception"));

                System.out.println("\n✅ Logger closed automatically by try-with-resources");

            } catch (IOException e) {
                System.err.println("[SECURE] Error: " + e.getMessage());
            }
        }

        public static void demonstrateBufferedLogging() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("BUFFERED LOGGING DEMONSTRATION");
            System.out.println("=".repeat(60));
            
            // SECURE: Buffered logger with auto-flush
            try (BufferedSecureLogger logger = new BufferedSecureLogger(
                    LOG_DIR + "/buffered.log", 5)) {
                
                System.out.println("Writing 3 messages (buffer size 5) - no flush yet");
                for (int i = 1; i <= 3; i++) {
                    logger.log("Message " + i);
                    System.out.println("  Buffered: Message " + i");
                }
                
                System.out.println("\nWriting 3 more messages (will auto-flush)");
                for (int i = 4; i <= 6; i++) {
                    logger.log("Message " + i);
                    System.out.println("  Buffered: Message " + i + 
                        (i == 6 ? " - triggered flush" : ""));
                }
                
                System.out.println("\n✅ Logger will flush remaining buffer on close");
            } catch (IOException e) {
                System.err.println("[BUFFERED] Error: " + e.getMessage());
            }
        }

        public static void demonstrateBatchProcessing() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("BATCH PROCESSING DEMONSTRATION");
            System.out.println("=".repeat(60));

            try (SecureLogger logger = new SecureLogger(LOG_DIR + "/" + APP_LOG)) {

                List<String> batch = new ArrayList<>();
                for (int i = 1; i <= 10; i++) {
                    batch.add("Batch message " + i);
                }

                System.out.println("Writing " + batch.size() + " messages in batch...");
                logger.writeBatch(batch);

                System.out.println("✅ Batch written successfully");

            } catch (IOException e) {
                System.err.println("[BATCH] Error: " + e.getMessage());
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-460: Improper Cleanup on Thrown Exception");
        System.out.println("================================================\n");

        // Create log directory
        new File(LOG_DIR).mkdirs();

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE logging (CWE-460)");
            System.out.println("  2. Demonstrate INSECURE finalize (BAD practice)");
            System.out.println("  3. Demonstrate SECURE logging (try-with-resources)");
            System.out.println("  4. Demonstrate buffered logging");
            System.out.println("  5. Demonstrate batch processing");
            System.out.println("  6. Compare approaches");
            System.out.println("  7. Show security analysis");
            System.out.println("  8. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    LoggerDemonstrator.demonstrateInsecureLogging();
                    break;

                case "2":
                    LoggerDemonstrator.demonstrateInsecureFinalize();
                    break;

                case "3":
                    LoggerDemonstrator.demonstrateSecureLogging();
                    break;

                case "4":
                    LoggerDemonstrator.demonstrateBufferedLogging();
                    break;

                case "5":
                    LoggerDemonstrator.demonstrateBatchProcessing();
                    break;

                case "6":
                    compareApproaches();
                    break;

                case "7":
                    showSecurityAnalysis();
                    break;

                case "8":
                    System.out.println("\nExiting...");
                    cleanup();
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void compareApproaches() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("APPROACH COMPARISON");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE APPROACH:");
        System.out.println("  • Manual close() in finally block");
        System.out.println("  • Easy to forget or miss");
        System.out.println("  • Exception can skip cleanup");
        System.out.println("  • Resource leaks common");
        System.out.println("  • Hard to maintain");

        System.out.println("\n⚠️ INSECURE FINALIZE:");
        System.out.println("  • Unpredictable execution");
        System.out.println("  • Deprecated in Java 9+");
        System.out.println("  • Performance overhead");
        System.out.println("  • Not guaranteed to run");

        System.out.println("\n✅ SECURE APPROACHES:");

        System.out.println("\n  1. Try-with-Resources:");
        System.out.println("     • Automatic cleanup");
        System.out.println("     • Guaranteed close()");
        System.out.println("     • Handles exceptions properly");
        System.out.println("     • Clean, readable code");

        System.out.println("\n  2. Finally Block with null check:");
        System.out.println("     • Manual but reliable");
        System.out.println("     • More verbose");
        System.out.println("     • Still error-prone");

        System.out.println("\n  3. AutoCloseable implementation:");
        System.out.println("     • Works with try-with-resources");
        System.out.println("     • Cleanup logic in one place");
        System.out.println("     • Reusable pattern");

        System.out.println("\n📊 CLEANUP GUARANTEES:");
        System.out.println("  Method              | Guaranteed | Simple | Modern");
        System.out.println("  --------------------|------------|--------|--------");
        System.out.println("  No cleanup          |     ❌     |   ✓    |   ❌");
        System.out.println("  finally block       |     ✓      |   ⚠️   |   ⚠️");
        System.out.println("  finalize()          |     ❌     |   ✓    |   ❌");
        System.out.println("  try-with-resources  |     ✓      |   ✓    |   ✓");
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-460");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-460 VULNERABLE):");
        System.out.println("  1. No Cleanup on Exception:");
        System.out.println("     • File handles left open");
        System.out.println("     • Resource exhaustion");
        System.out.println("     • Memory leaks");

        System.out.println("\n  2. Multiple Exit Points:");
        System.out.println("     • Some paths skip cleanup");
        System.out.println("     • Inconsistent state");
        System.out.println("     • Hard to debug");

        System.out.println("\n  3. Using finalize():");
        System.out.println("     • Unpredictable timing");
        System.out.println("     • Deprecated and unsafe");
        System.out.println("     • Performance impact");

        System.out.println("\n  4. Consequences:");
        System.out.println("     • Too many open files");
        System.out.println("     • Application crash");
        System.out.println("     • Data loss");
        System.out.println("     • Denial of service");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Try-with-Resources:");
        System.out.println("     • Automatic cleanup");
        System.out.println("     • Guaranteed close()");
        System.out.println("     • Exception-safe");

        System.out.println("\n  2. Proper Exception Handling:");
        System.out.println("     • Cleanup in finally blocks");
        System.out.println("     • Null checks before close");
        System.out.println("     • Log cleanup errors");

        System.out.println("\n  3. AutoCloseable Interface:");
        System.out.println("     • Standard pattern");
        System.out.println("     • Works with try-with-resources");
        System.out.println("     • Reusable");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Always use try-with-resources for Closeable resources");
        System.out.println("  2. Never rely on finalize()");
        System.out.println("  3. Close resources in finally block if not using try-with-resources");
        System.out.println("  4. Check for null before closing");
        System.out.println("  5. Log but don't throw from finally blocks");
        System.out.println("  6. Keep resource acquisition and release in same scope");
        System.out.println("  7. Use AutoCloseable for custom resources");
        System.out.println("  8. Test exception paths");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use try-with-resources for all Closeable resources");
        System.out.println("  • Implement AutoCloseable for custom resources");
        System.out.println("  • Never use finalize()");
        System.out.println("  • Always test exception scenarios");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  FileWriter fw = new FileWriter(\"log.txt\");");
        System.out.println("  fw.write(data); // May throw");
        System.out.println("  fw.close(); // May not be reached");
        System.out.println("");
        System.out.println("  // SECURE (old style)");
        System.out.println("  FileWriter fw = null;");
        System.out.println("  try {");
        System.out.println("      fw = new FileWriter(\"log.txt\");");
        System.out.println("      fw.write(data);");
        System.out.println("  } finally {");
        System.out.println("      if (fw != null) fw.close();");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // SECURE (modern)");
        System.out.println("  try (FileWriter fw = new FileWriter(\"log.txt\")) {");
        System.out.println("      fw.write(data);");
        System.out.println("  } // Auto-closed");
    }

    private static void cleanup() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("CLEANUP");
        System.out.println("=".repeat(50));

        // In a real app, you might want to archive or rotate logs
        System.out.println("Log files are in: " + LOG_DIR + "/");
    }
}