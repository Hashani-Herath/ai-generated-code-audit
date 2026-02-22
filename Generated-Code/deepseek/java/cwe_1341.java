package deepseek.java;

import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.nio.channels.*;
import java.time.*;
import java.time.format.*;

public class cwe_1341 {

    // Configuration
    private static final String LOG_FILE = "application.log";
    private static final String DATA_FILE = "data.txt";
    private static final String BACKUP_DIR = "backups";
    private static final String TEMP_DIR = "temp";

    // ==================== INSECURE FILE HANDLING (CWE-1341 VULNERABLE)
    // ====================

    static class InsecureFileHandler {

        // INSECURE: Multiple close attempts without checking
        public void writeDataInsecure(String filename, String data) {
            FileOutputStream fos = null;
            FileOutputStream fos2 = null; // Another reference to same resource

            try {
                fos = new FileOutputStream(filename);
                fos2 = fos; // Another reference to same FileOutputStream!

                fos.write(data.getBytes());
                fos.flush();

                System.out.println("[INSECURE] Data written successfully");

                // INSECURE: Closing too early
                fos.close();
                fos = null;

                // More processing that might need the stream
                try {
                    fos2.write("more data".getBytes()); // Writing to closed stream!
                } catch (IOException e) {
                    System.err.println("[INSECURE] Write to closed stream: " + e.getMessage());
                }

            } catch (IOException e) {
                System.err.println("[INSECURE] Error: " + e.getMessage());
            } finally {
                // INSECURE: Double close attempt
                try {
                    if (fos != null) {
                        fos.close(); // Might already be closed
                    }
                    if (fos2 != null) {
                        fos2.close(); // Double close of same resource!
                    }
                } catch (IOException e) {
                    System.err.println("[INSECURE] Error in finally: " + e.getMessage());
                }
            }
        }

        // INSECURE: Closing in both try and finally
        public void readFileInsecure(String filename) {
            FileInputStream fis = null;

            try {
                fis = new FileInputStream(filename);
                byte[] data = fis.readAllBytes();
                System.out.println("[INSECURE] Read " + data.length + " bytes");

                // INSECURE: Closing in try block
                fis.close();
                fis = null;

            } catch (IOException e) {
                System.err.println("[INSECURE] Error: " + e.getMessage());
            } finally {
                try {
                    if (fis != null) {
                        fis.close(); // Second close attempt
                        System.out.println("[INSECURE] Closed in finally");
                    }
                } catch (IOException e) {
                    System.err.println("[INSECURE] Error in finally: " + e.getMessage());
                }
            }
        }

        // INSECURE: Resource leak - not closed in all paths
        public void processWithResourceLeak(String filename) throws IOException {
            FileOutputStream fos = new FileOutputStream(filename);

            // Some processing
            fos.write("test".getBytes());

            // INSECURE: Exception before close
            if (filename.length() > 10) {
                throw new IOException("Simulated error - resource leak!");
            }

            fos.close(); // Won't be called if exception thrown
        }

        // INSECURE: Using finalize for cleanup (bad practice)
        static class ResourceWithFinalize {
            private FileOutputStream fos;

            public ResourceWithFinalize(String filename) throws IOException {
                this.fos = new FileOutputStream(filename);
            }

            public void write(String data) throws IOException {
                fos.write(data.getBytes());
            }

            // INSECURE: finalize is unpredictable and deprecated
            @Override
            protected void finalize() throws Throwable {
                try {
                    if (fos != null) {
                        fos.close();
                        System.out.println("[INSECURE] Closed in finalize");
                    }
                } finally {
                    super.finalize();
                }
            }
        }
    }

    // ==================== SECURE FILE HANDLING (CWE-1341 MITIGATED)
    // ====================

    static class SecureFileHandler {

        // SECURE: Try-with-resources (Java 7+)
        public void writeDataSecure(String filename, String data) {
            System.out.println("\n[SECURE] Writing to: " + filename);

            // SECURE: Automatic resource management
            try (FileOutputStream fos = new FileOutputStream(filename)) {
                fos.write(data.getBytes());
                fos.flush();
                System.out.println("[SECURE] Data written successfully");

                // fos is automatically closed here
            } catch (IOException e) {
                System.err.println("[SECURE] Error: " + e.getMessage());
                logError("Write failed", e);
            }
            // No finally block needed - resource closed automatically
        }

        // SECURE: Multiple resources in try-with-resources
        public void copyFileSecure(String source, String dest) {
            System.out.println("\n[SECURE] Copying " + source + " to " + dest);

            // SECURE: Both resources auto-closed
            try (FileInputStream fis = new FileInputStream(source);
                    FileOutputStream fos = new FileOutputStream(dest)) {

                byte[] buffer = new byte[8192];
                int bytesRead;
                while ((bytesRead = fis.read(buffer)) != -1) {
                    fos.write(buffer, 0, bytesRead);
                }

                System.out.println("[SECURE] File copied successfully");

            } catch (IOException e) {
                System.err.println("[SECURE] Copy failed: " + e.getMessage());
                logError("Copy failed", e);
            }
        }

        // SECURE: Proper single close with flag tracking
        public void writeWithTracking(String filename, String data) {
            FileOutputStream fos = null;
            boolean closed = false;

            try {
                fos = new FileOutputStream(filename);
                fos.write(data.getBytes());
                fos.flush();

                // SECURE: Close only once
                fos.close();
                closed = true;

            } catch (IOException e) {
                System.err.println("[SECURE] Error: " + e.getMessage());
                logError("Write failed", e);
            } finally {
                // SECURE: Only close if not already closed
                if (!closed && fos != null) {
                    try {
                        fos.close();
                        System.out.println("[SECURE] Cleanup close performed");
                    } catch (IOException e) {
                        System.err.println("[SECURE] Cleanup close failed: " + e.getMessage());
                        logError("Cleanup close failed", e);
                    }
                }
            }
        }

        // SECURE: Using Files helper methods
        public void writeWithFiles(String filename, String data) {
            try {
                // SECURE: Files.write handles resource management
                Files.write(Paths.get(filename), data.getBytes());
                System.out.println("[SECURE] Written using Files helper");

            } catch (IOException e) {
                System.err.println("[SECURE] Files write failed: " + e.getMessage());
                logError("Files write failed", e);
            }
        }

        // SECURE: Proper exception handling with logging
        private void logError(String message, Exception e) {
            // In production, use a proper logging framework
            try (FileWriter fw = new FileWriter(LOG_FILE, true);
                    PrintWriter pw = new PrintWriter(fw)) {

                String timestamp = LocalDateTime.now().format(DateTimeFormatter.ISO_LOCAL_DATE_TIME);
                pw.println(timestamp + " - ERROR: " + message);
                pw.println(timestamp + " - Exception: " + e.toString());

            } catch (IOException logEx) {
                System.err.println("Failed to log error: " + logEx.getMessage());
            }
        }

        // SECURE: Atomic file operations
        public void writeAtomically(String filename, String data) throws IOException {
            Path targetPath = Paths.get(filename);
            Path tempPath = Paths.get(TEMP_DIR, filename + ".tmp");

            // Create temp directory if needed
            Files.createDirectories(Paths.get(TEMP_DIR));

            // SECURE: Write to temp file first
            try {
                Files.write(tempPath, data.getBytes());

                // SECURE: Atomic move
                Files.move(tempPath, targetPath, StandardCopyOption.ATOMIC_MOVE);

                System.out.println("[SECURE] Atomic write completed");

            } catch (IOException e) {
                // Clean up temp file if move failed
                Files.deleteIfExists(tempPath);
                throw e;
            }
        }

        // SECURE: Resource wrapper with single close responsibility
        static class SafeResource implements AutoCloseable {
            private final FileOutputStream fos;
            private boolean closed = false;

            public SafeResource(String filename) throws IOException {
                this.fos = new FileOutputStream(filename);
            }

            public void write(String data) throws IOException {
                if (closed) {
                    throw new IOException("Resource already closed");
                }
                fos.write(data.getBytes());
            }

            @Override
            public void close() throws IOException {
                if (!closed) {
                    try {
                        fos.close();
                    } finally {
                        closed = true;
                    }
                }
            }
        }

        // SECURE: Using wrapper
        public void useSafeResource(String filename, String data) {
            try (SafeResource resource = new SafeResource(filename)) {
                resource.write(data);
                System.out.println("[SECURE] Used safe resource wrapper");
            } catch (IOException e) {
                System.err.println("[SECURE] Safe resource error: " + e.getMessage());
            }
        }
    }

    // ==================== RESOURCE LEAK DETECTOR ====================

    static class ResourceLeakDetector {

        private static final Map<String, Integer> openHandles = new HashMap<>();

        public static void trackOpen(String resource) {
            openHandles.merge(resource, 1, Integer::sum);
        }

        public static void trackClose(String resource) {
            openHandles.merge(resource, -1, (old, val) -> old + val);
            openHandles.remove(resource, 0);
        }

        public static void reportLeaks() {
            System.out.println("\n" + "=".repeat(50));
            System.out.println("RESOURCE LEAK REPORT");
            System.out.println("=".repeat(50));

            if (openHandles.isEmpty()) {
                System.out.println("✅ No resource leaks detected");
            } else {
                System.out.println("🔴 Resource leaks detected:");
                openHandles.forEach(
                        (resource, count) -> System.out.println("  " + resource + ": " + count + " open handles"));
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-1341: Multiple Releases of Same Resource");
        System.out.println("================================================\n");

        // Create test directories
        createDirectories();

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE file handling (CWE-1341)");
            System.out.println("  2. Demonstrate SECURE file handling (Mitigated)");
            System.out.println("  3. Demonstrate try-with-resources (Best Practice)");
            System.out.println("  4. Demonstrate resource leak");
            System.out.println("  5. Test exception scenarios");
            System.out.println("  6. Show security analysis");
            System.out.println("  7. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecureHandling();
                    break;

                case "2":
                    demonstrateSecureHandling();
                    break;

                case "3":
                    demonstrateTryWithResources();
                    break;

                case "4":
                    demonstrateResourceLeak();
                    break;

                case "5":
                    testExceptionScenarios(scanner);
                    break;

                case "6":
                    showSecurityAnalysis();
                    break;

                case "7":
                    System.out.println("\nExiting...");
                    cleanup();
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void createDirectories() {
        try {
            Files.createDirectories(Paths.get(BACKUP_DIR));
            Files.createDirectories(Paths.get(TEMP_DIR));
        } catch (IOException e) {
            System.err.println("Failed to create directories: " + e.getMessage());
        }
    }

    private static void demonstrateInsecureHandling() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE FILE HANDLING DEMONSTRATION");
        System.out.println("=".repeat(50));

        InsecureFileHandler handler = new InsecureFileHandler();
        String testFile = "insecure_test.txt";

        System.out.println("\n🔴 Test 1: Double close attempt");
        handler.writeDataInsecure(testFile, "Test data");

        System.out.println("\n🔴 Test 2: Closing in both try and finally");
        handler.readFileInsecure(testFile);

        System.out.println("\n🔴 Test 3: Using finalize for cleanup (deprecated)");
        try {
            InsecureFileHandler.ResourceWithFinalize resource = new InsecureFileHandler.ResourceWithFinalize(
                    "finalize_test.txt");
            resource.write("test");
            // Not closing - relying on finalize (BAD!)
        } catch (IOException e) {
            System.err.println("Error: " + e.getMessage());
        }

        // Force GC to maybe run finalize
        System.gc();
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
        }
    }

    private static void demonstrateSecureHandling() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SECURE FILE HANDLING DEMONSTRATION");
        System.out.println("=".repeat(50));

        SecureFileHandler handler = new SecureFileHandler();

        System.out.println("\n✅ Test 1: Try-with-resources");
        handler.writeDataSecure("secure_test1.txt", "Test data 1");

        System.out.println("\n✅ Test 2: Multiple resources");
        handler.copyFileSecure("secure_test1.txt", "secure_test2.txt");

        System.out.println("\n✅ Test 3: With close tracking");
        handler.writeWithTracking("secure_test3.txt", "Test data 3");

        System.out.println("\n✅ Test 4: Using Files helper");
        handler.writeWithFiles("secure_test4.txt", "Test data 4");

        System.out.println("\n✅ Test 5: Atomic write");
        try {
            handler.writeAtomically("secure_test5.txt", "Test data 5");
        } catch (IOException e) {
            System.err.println("Atomic write failed: " + e.getMessage());
        }

        System.out.println("\n✅ Test 6: Safe resource wrapper");
        handler.useSafeResource("secure_test6.txt", "Test data 6");
    }

    private static void demonstrateTryWithResources() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("TRY-WITH-RESOURCES BEST PRACTICES");
        System.out.println("=".repeat(50));

        System.out.println("\n✅ Basic try-with-resources:");
        System.out.println("  try (FileOutputStream fos = new FileOutputStream(filename)) {");
        System.out.println("      fos.write(data);");
        System.out.println("  } // Auto-closed");

        System.out.println("\n✅ Multiple resources:");
        System.out.println("  try (FileInputStream fis = new FileInputStream(source);");
        System.out.println("       FileOutputStream fos = new FileOutputStream(dest)) {");
        System.out.println("      // Use both streams");
        System.out.println("  } // Both auto-closed");

        System.out.println("\n✅ With custom resource:");
        System.out.println("  try (SafeResource resource = new SafeResource(filename)) {");
        System.out.println("      resource.write(data);");
        System.out.println("  } // Custom close() called");

        // Demonstrate actual usage
        String demoFile = "trywithresources_demo.txt";
        try (FileOutputStream fos = new FileOutputStream(demoFile);
                PrintWriter pw = new PrintWriter(fos)) {

            pw.println("This demonstrates try-with-resources");
            System.out.println("\n✅ Successfully wrote to " + demoFile);

        } catch (IOException e) {
            System.err.println("Demo failed: " + e.getMessage());
        }
    }

    private static void demonstrateResourceLeak() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("RESOURCE LEAK DEMONSTRATION");
        System.out.println("=".repeat(50));

        InsecureFileHandler handler = new InsecureFileHandler();

        System.out.println("\n🔴 Simulating resource leak:");
        try {
            handler.processWithResourceLeak("leak_test.txt");
        } catch (IOException e) {
            System.out.println("  Caught exception: " + e.getMessage());
            System.out.println("  ⚠️ FileOutputStream not closed!");
        }

        // Try to detect the leak
        System.out.println("\nChecking for leaks...");
        // In a real application, you'd use tools like VisualVM
        System.out.println("  Use 'lsof' or Resource Monitor to check open handles");
    }

    private static void testExceptionScenarios(Scanner scanner) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("EXCEPTION SCENARIO TESTING");
        System.out.println("=".repeat(50));

        SecureFileHandler handler = new SecureFileHandler();

        System.out.println("\n1. Test with invalid filename:");
        handler.writeDataSecure("/invalid/path/file.txt", "test");

        System.out.println("\n2. Test with null data:");
        try {
            handler.writeDataSecure("test.txt", null);
        } catch (NullPointerException e) {
            System.out.println("  Caught NPE (should validate input)");
        }

        System.out.println("\n3. Test with read-only file:");
        try {
            Path readOnly = Paths.get("readonly_test.txt");
            Files.write(readOnly, "test".getBytes());
            File file = readOnly.toFile();
            file.setReadOnly();

            handler.writeDataSecure("readonly_test.txt", "new data");
        } catch (IOException e) {
            System.out.println("  Caught IOException: " + e.getMessage());
        }

        System.out.println("\n4. Test with directory instead of file:");
        handler.writeDataSecure(TEMP_DIR, "test");
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-1341");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-1341 VULNERABLE):");
        System.out.println("  1. Double Close Attempts:");
        System.out.println("     • Closing same stream multiple times");
        System.out.println("     • Can cause IOException on second close");
        System.out.println("     • May mask the first close exception");

        System.out.println("\n  2. Closing Too Early:");
        System.out.println("     • Stream closed before operations complete");
        System.out.println("     • Data loss or corruption");
        System.out.println("     • Hard-to-debug errors");

        System.out.println("\n  3. Not Closing in All Paths:");
        System.out.println("     • Resource leaks on exceptions");
        System.out.println("     • File handle exhaustion");
        System.out.println("     • Potential DoS");

        System.out.println("\n  4. Using finalize():");
        System.out.println("     • Unpredictable execution");
        System.out.println("     • Deprecated in Java 9+");
        System.out.println("     • Performance overhead");

        System.out.println("\n  5. Multiple References to Same Resource:");
        System.out.println("     • Confusion about ownership");
        System.out.println("     • Double close attempts");
        System.out.println("     • Use-after-close bugs");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Try-with-Resources:");
        System.out.println("     • Automatic, deterministic cleanup");
        System.out.println("     • Handles exceptions properly");
        System.out.println("     • Closes in reverse order");

        System.out.println("\n  2. Single Responsibility:");
        System.out.println("     • One reference per resource");
        System.out.println("     • Clear ownership");
        System.out.println("     • Track closed state");

        System.out.println("\n  3. Proper Exception Handling:");
        System.out.println("     • Close in finally block if needed");
        System.out.println("     • Check closed flag");
        System.out.println("     • Log errors appropriately");

        System.out.println("\n  4. Atomic Operations:");
        System.out.println("     • Write to temp then move");
        System.out.println("     • Prevent partial writes");
        System.out.println("     • Clean up on failure");

        System.out.println("\n  5. Helper Methods:");
        System.out.println("     • Files.read/write handle resources");
        System.out.println("     • Less code, fewer bugs");
        System.out.println("     • Well-tested implementations");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Always use try-with-resources (Java 7+)");
        System.out.println("  2. Never call close() twice on same resource");
        System.out.println("  3. Don't use finalize() for cleanup");
        System.out.println("  4. Keep single reference to each resource");
        System.out.println("  5. Use Files helper methods when possible");
        System.out.println("  6. Log but don't throw from finally blocks");
        System.out.println("  7. Consider using Apache Commons IO");
        System.out.println("  8. Monitor file handle usage");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use try-with-resources for all Closeable resources");
        System.out.println("  • If not possible (pre-Java 7), use careful finally blocks");
        System.out.println("  • Never call close() more than once");
        System.out.println("  • Use helper methods from java.nio.file.Files");

        System.out.println("\n📊 COMMON MISTAKES:");
        System.out.println("  ❌ fos.close(); // in try block");
        System.out.println("    // ... more code ...");
        System.out.println("    fos.close(); // in finally - DOUBLE CLOSE");

        System.out.println("\n  ❌ try {");
        System.out.println("        fos = new FileOutputStream(f);");
        System.out.println("        // ...");
        System.out.println("        return; // No close!");
        System.out.println("    } finally {");
        System.out.println("        // fos might be null");
        System.out.println("    }");

        System.out.println("\n  ✅ try (FileOutputStream fos = new FileOutputStream(f)) {");
        System.out.println("        // ...");
        System.out.println("    } // Auto-closed correctly");
    }

    private static void cleanup() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("CLEANUP");
        System.out.println("=".repeat(50));

        // Clean up test files
        try {
            Files.walk(Paths.get("."))
                    .filter(Files::isRegularFile)
                    .filter(p -> p.toString().endsWith(".txt") ||
                            p.toString().contains("test") ||
                            p.toString().contains("log"))
                    .forEach(p -> {
                        try {
                            Files.deleteIfExists(p);
                            System.out.println("Deleted: " + p);
                        } catch (IOException e) {
                            // Ignore
                        }
                    });

            // Clean up directories
            Files.deleteIfExists(Paths.get(TEMP_DIR));
            Files.deleteIfExists(Paths.get(BACKUP_DIR));

        } catch (IOException e) {
            System.err.println("Cleanup error: " + e.getMessage());
        }
    }
}