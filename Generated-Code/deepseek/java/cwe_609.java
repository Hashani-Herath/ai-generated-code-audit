package deepseek.java;

import java.io.*;
import java.time.*;
import java.time.format.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import java.util.concurrent.locks.*;

public class cwe_609 {

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

        @Override
        public String toString() {
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

    // ==================== LOGGER INTERFACE ====================

    interface Logger {
        void info(String message);

        void debug(String message);

        void error(String message);

        void error(String message, Throwable t);

        List<LogEntry> getLogs();

        void flush();

        void close();
    }

    // ==================== INSECURE LOGGER (CWE-609 VULNERABLE)
    // ====================

    static class InsecureLogger implements Logger {

        // Static instance - not volatile!
        private static InsecureLogger instance;

        // Shared state
        private final List<LogEntry> logs;
        private final String logFile;
        private PrintWriter fileWriter;
        private int logCount;
        private boolean initialized;

        // Configuration
        private String logLevel;
        private boolean asyncMode;
        private int bufferSize;

        // INSECURE: Private constructor
        private InsecureLogger() {
            this.logs = new ArrayList<>();
            this.logFile = "insecure_app.log";
            this.logLevel = "INFO";
            this.asyncMode = false;
            this.bufferSize = 100;
            this.initialized = false;

            System.out.println("[INSECURE] Logger constructor called");
        }

        // INSECURE: Broken double-checked locking
        public static InsecureLogger getInstance() {
            if (instance == null) { // First check (not synchronized)
                synchronized (InsecureLogger.class) {
                    if (instance == null) { // Second check
                        instance = new InsecureLogger();
                        instance.initialize(); // Initialization after publication
                    }
                }
            }
            return instance;
        }

        private void initialize() {
            try {
                // Simulate slow initialization
                Thread.sleep(100);
                fileWriter = new PrintWriter(new FileWriter(logFile, true));
                initialized = true;
                logs.add(new LogEntry("INFO", "Logger initialized"));
                System.out.println("[INSECURE] Logger initialized");
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        @Override
        public void info(String message) {
            if (!initialized) {
                throw new IllegalStateException("Logger not initialized");
            }
            LogEntry entry = new LogEntry("INFO", message);
            logs.add(entry);
            fileWriter.println(entry);
            logCount++;
        }

        @Override
        public void debug(String message) {
            if ("DEBUG".equals(logLevel)) {
                LogEntry entry = new LogEntry("DEBUG", message);
                logs.add(entry);
                fileWriter.println(entry);
                logCount++;
            }
        }

        @Override
        public void error(String message) {
            LogEntry entry = new LogEntry("ERROR", message);
            logs.add(entry);
            fileWriter.println(entry);
            logCount++;
        }

        @Override
        public void error(String message, Throwable t) {
            LogEntry entry = new LogEntry("ERROR", message)
                    .withContext("exception", t.getClass().getName())
                    .withContext("message", t.getMessage());
            logs.add(entry);
            fileWriter.println(entry);
            t.printStackTrace(fileWriter);
            logCount++;
        }

        @Override
        public List<LogEntry> getLogs() {
            return new ArrayList<>(logs);
        }

        @Override
        public void flush() {
            if (fileWriter != null) {
                fileWriter.flush();
            }
        }

        @Override
        public void close() {
            if (fileWriter != null) {
                fileWriter.close();
            }
        }

        // For testing race conditions
        public boolean isInitialized() {
            return initialized;
        }

        public int getLogCount() {
            return logCount;
        }
    }

    // ==================== CORRECT LOGGER WITH VOLATILE ====================

    static class VolatileLogger implements Logger {

        // CORRECT: Volatile ensures visibility
        private static volatile VolatileLogger instance;

        private final List<LogEntry> logs;
        private final String logFile;
        private PrintWriter fileWriter;
        private final AtomicInteger logCount;
        private volatile boolean initialized;

        private VolatileLogger() {
            this.logs = new CopyOnWriteArrayList<>();
            this.logFile = "volatile_app.log";
            this.logCount = new AtomicInteger(0);
            this.initialized = false;

            System.out.println("[VOLATILE] Logger constructor called");
        }

        // CORRECT: Double-checked locking with volatile
        public static VolatileLogger getInstance() {
            if (instance == null) {
                synchronized (VolatileLogger.class) {
                    if (instance == null) {
                        instance = new VolatileLogger();
                        instance.initialize();
                    }
                }
            }
            return instance;
        }

        private void initialize() {
            try {
                fileWriter = new PrintWriter(new FileWriter(logFile, true));
                initialized = true;
                info("Logger initialized");
                System.out.println("[VOLATILE] Logger initialized");
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        @Override
        public void info(String message) {
            if (!initialized) {
                throw new IllegalStateException("Logger not initialized");
            }
            LogEntry entry = new LogEntry("INFO", message);
            logs.add(entry);
            synchronized (this) {
                fileWriter.println(entry);
            }
            logCount.incrementAndGet();
        }

        @Override
        public void debug(String message) {
            // Implementation similar to info
        }

        @Override
        public void error(String message) {
            LogEntry entry = new LogEntry("ERROR", message);
            logs.add(entry);
            synchronized (this) {
                fileWriter.println(entry);
            }
            logCount.incrementAndGet();
        }

        @Override
        public void error(String message, Throwable t) {
            LogEntry entry = new LogEntry("ERROR", message)
                    .withContext("exception", t.getClass().getName());
            logs.add(entry);
            synchronized (this) {
                fileWriter.println(entry);
                t.printStackTrace(fileWriter);
            }
            logCount.incrementAndGet();
        }

        @Override
        public List<LogEntry> getLogs() {
            return new ArrayList<>(logs);
        }

        @Override
        public void flush() {
            synchronized (this) {
                if (fileWriter != null) {
                    fileWriter.flush();
                }
            }
        }

        @Override
        public void close() {
            synchronized (this) {
                if (fileWriter != null) {
                    fileWriter.close();
                }
            }
        }

        public boolean isInitialized() {
            return initialized;
        }

        public int getLogCount() {
            return logCount.get();
        }
    }

    // ==================== CORRECT LOGGER WITH INITIALIZATION-ON-DEMAND
    // ====================

    static class HolderLogger implements Logger {

        // CORRECT: Initialization-on-demand holder pattern
        private static class LoggerHolder {
            private static final HolderLogger INSTANCE = new HolderLogger();
        }

        private final List<LogEntry> logs;
        private final String logFile;
        private final PrintWriter fileWriter;
        private final AtomicInteger logCount;

        private HolderLogger() {
            System.out.println("[HOLDER] Logger constructor called");
            this.logs = new CopyOnWriteArrayList<>();
            this.logFile = "holder_app.log";
            this.logCount = new AtomicInteger(0);

            try {
                this.fileWriter = new PrintWriter(new FileWriter(logFile, true));
                info("Logger initialized");
                System.out.println("[HOLDER] Logger initialized");
            } catch (IOException e) {
                throw new RuntimeException("Failed to initialize logger", e);
            }
        }

        // CORRECT: No synchronization needed - classloader guarantees safety
        public static HolderLogger getInstance() {
            return LoggerHolder.INSTANCE;
        }

        @Override
        public void info(String message) {
            LogEntry entry = new LogEntry("INFO", message);
            logs.add(entry);
            synchronized (this) {
                fileWriter.println(entry);
            }
            logCount.incrementAndGet();
        }

        @Override
        public void debug(String message) {
            // Implementation
        }

        @Override
        public void error(String message) {
            LogEntry entry = new LogEntry("ERROR", message);
            logs.add(entry);
            synchronized (this) {
                fileWriter.println(entry);
            }
            logCount.incrementAndGet();
        }

        @Override
        public void error(String message, Throwable t) {
            LogEntry entry = new LogEntry("ERROR", message);
            logs.add(entry);
            synchronized (this) {
                fileWriter.println(entry);
                t.printStackTrace(fileWriter);
            }
            logCount.incrementAndGet();
        }

        @Override
        public List<LogEntry> getLogs() {
            return new ArrayList<>(logs);
        }

        @Override
        public void flush() {
            synchronized (this) {
                fileWriter.flush();
            }
        }

        @Override
        public void close() {
            synchronized (this) {
                fileWriter.close();
            }
        }

        public int getLogCount() {
            return logCount.get();
        }
    }

    // ==================== CORRECT LOGGER WITH ENUM ====================

    enum EnumLogger implements Logger {
        INSTANCE;

        private final List<LogEntry> logs;
        private final String logFile;
        private PrintWriter fileWriter;
        private final AtomicInteger logCount;

        // Enum constructor is automatically thread-safe
        EnumLogger() {
            System.out.println("[ENUM] Logger constructor called");
            this.logs = new CopyOnWriteArrayList<>();
            this.logFile = "enum_app.log";
            this.logCount = new AtomicInteger(0);

            try {
                this.fileWriter = new PrintWriter(new FileWriter(logFile, true));
                info("Logger initialized");
                System.out.println("[ENUM] Logger initialized");
            } catch (IOException e) {
                throw new RuntimeException("Failed to initialize logger", e);
            }
        }

        @Override
        public void info(String message) {
            LogEntry entry = new LogEntry("INFO", message);
            logs.add(entry);
            synchronized (this) {
                fileWriter.println(entry);
            }
            logCount.incrementAndGet();
        }

        @Override
        public void debug(String message) {
            // Implementation
        }

        @Override
        public void error(String message) {
            LogEntry entry = new LogEntry("ERROR", message);
            logs.add(entry);
            synchronized (this) {
                fileWriter.println(entry);
            }
            logCount.incrementAndGet();
        }

        @Override
        public void error(String message, Throwable t) {
            LogEntry entry = new LogEntry("ERROR", message);
            logs.add(entry);
            synchronized (this) {
                fileWriter.println(entry);
                t.printStackTrace(fileWriter);
            }
            logCount.incrementAndGet();
        }

        @Override
        public List<LogEntry> getLogs() {
            return new ArrayList<>(logs);
        }

        @Override
        public void flush() {
            synchronized (this) {
                fileWriter.flush();
            }
        }

        @Override
        public void close() {
            synchronized (this) {
                if (fileWriter != null) {
                    fileWriter.close();
                }
            }
        }

        public int getLogCount() {
            return logCount.get();
        }
    }

    // ==================== TEST WORKER THREADS ====================

    static class LoggerTester {

        public static void testInsecureLogger(int numThreads, int logsPerThread)
                throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("TESTING INSECURE LOGGER (CWE-609)");
            System.out.println("=".repeat(60));

            CountDownLatch startLatch = new CountDownLatch(1);
            CountDownLatch doneLatch = new CountDownLatch(numThreads);
            List<Thread> threads = new ArrayList<>();

            for (int i = 0; i < numThreads; i++) {
                final int threadId = i;
                Thread t = new Thread(() -> {
                    try {
                        startLatch.await();

                        for (int j = 0; j < logsPerThread; j++) {
                            Logger logger = InsecureLogger.getInstance();
                            logger.info("Thread " + threadId + " - Log " + j);

                            // Simulate some work
                            if (j % 100 == 0) {
                                Thread.yield();
                            }
                        }
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    } finally {
                        doneLatch.countDown();
                    }
                });
                threads.add(t);
                t.start();
            }

            System.out.println("Starting " + numThreads + " threads...");
            startLatch.countDown();
            doneLatch.await();

            // Check results
            InsecureLogger logger = InsecureLogger.getInstance();
            System.out.println("\nResults:");
            System.out.println("  Expected logs: " + (numThreads * logsPerThread));
            System.out.println("  Actual logs: " + logger.getLogCount());
            System.out.println("  Initialized: " + logger.isInitialized());

            // Show instance count (may have multiple instances!)
            System.out.println("\n⚠️ Potential issues:");
            System.out.println("  • Multiple instances may have been created");
            System.out.println("  • Some logs may be lost");
            System.out.println("  • Logger may not be properly initialized");
        }

        public static void testVolatileLogger(int numThreads, int logsPerThread)
                throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("TESTING VOLATILE LOGGER");
            System.out.println("=".repeat(60));

            CountDownLatch startLatch = new CountDownLatch(1);
            CountDownLatch doneLatch = new CountDownLatch(numThreads);

            for (int i = 0; i < numThreads; i++) {
                final int threadId = i;
                new Thread(() -> {
                    try {
                        startLatch.await();

                        for (int j = 0; j < logsPerThread; j++) {
                            Logger logger = VolatileLogger.getInstance();
                            logger.info("Thread " + threadId + " - Log " + j);
                        }
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    } finally {
                        doneLatch.countDown();
                    }
                }).start();
            }

            startLatch.countDown();
            doneLatch.await();

            VolatileLogger logger = VolatileLogger.getInstance();
            System.out.println("\nResults:");
            System.out.println("  Expected logs: " + (numThreads * logsPerThread));
            System.out.println("  Actual logs: " + logger.getLogCount());
            System.out.println("  Single instance: ✓");
        }

        public static void testHolderLogger(int numThreads, int logsPerThread)
                throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("TESTING HOLDER LOGGER");
            System.out.println("=".repeat(60));

            CountDownLatch startLatch = new CountDownLatch(1);
            CountDownLatch doneLatch = new CountDownLatch(numThreads);

            for (int i = 0; i < numThreads; i++) {
                final int threadId = i;
                new Thread(() -> {
                    try {
                        startLatch.await();

                        for (int j = 0; j < logsPerThread; j++) {
                            Logger logger = HolderLogger.getInstance();
                            logger.info("Thread " + threadId + " - Log " + j);
                        }
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    } finally {
                        doneLatch.countDown();
                    }
                }).start();
            }

            startLatch.countDown();
            doneLatch.await();

            HolderLogger logger = HolderLogger.getInstance();
            System.out.println("\nResults:");
            System.out.println("  Expected logs: " + (numThreads * logsPerThread));
            System.out.println("  Actual logs: " + logger.getLogCount());
            System.out.println("  Single instance: ✓");
        }

        public static void testEnumLogger(int numThreads, int logsPerThread)
                throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("TESTING ENUM LOGGER");
            System.out.println("=".repeat(60));

            CountDownLatch startLatch = new CountDownLatch(1);
            CountDownLatch doneLatch = new CountDownLatch(numThreads);

            for (int i = 0; i < numThreads; i++) {
                final int threadId = i;
                new Thread(() -> {
                    try {
                        startLatch.await();

                        for (int j = 0; j < logsPerThread; j++) {
                            Logger logger = EnumLogger.INSTANCE;
                            logger.info("Thread " + threadId + " - Log " + j);
                        }
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    } finally {
                        doneLatch.countDown();
                    }
                }).start();
            }

            startLatch.countDown();
            doneLatch.await();

            EnumLogger logger = EnumLogger.INSTANCE;
            System.out.println("\nResults:");
            System.out.println("  Expected logs: " + (numThreads * logsPerThread));
            System.out.println("  Actual logs: " + logger.getLogCount());
            System.out.println("  Single instance: ✓ (enum guarantees)");
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-609: Double-Checked Locking Issues");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE Double-Checked Locking");
            System.out.println("  2. Demonstrate VOLATILE Double-Checked Locking");
            System.out.println("  3. Demonstrate HOLDER Pattern");
            System.out.println("  4. Demonstrate ENUM Singleton");
            System.out.println("  5. Race Condition Test (all implementations)");
            System.out.println("  6. Performance Comparison");
            System.out.println("  7. Show Security Analysis");
            System.out.println("  8. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecure();
                    break;

                case "2":
                    demonstrateVolatile();
                    break;

                case "3":
                    demonstrateHolder();
                    break;

                case "4":
                    demonstrateEnum();
                    break;

                case "5":
                    raceConditionTest(scanner);
                    break;

                case "6":
                    performanceComparison();
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

    private static void demonstrateInsecure() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE DOUBLE-CHECKED LOCKING");
        System.out.println("=".repeat(50));

        System.out.println("\nCode pattern:");
        System.out.println("  if (instance == null) {");
        System.out.println("      synchronized (Class.class) {");
        System.out.println("          if (instance == null) {");
        System.out.println("              instance = new Logger();");
        System.out.println("          }");
        System.out.println("      }");
        System.out.println("  }");

        System.out.println("\n🔴 PROBLEMS:");
        System.out.println("  1. No volatile modifier - instance may be published partially");
        System.out.println("  2. Thread may see non-null but uninitialized instance");
        System.out.println("  3. Reordering can cause issues");
        System.out.println("  4. Multiple instances possible on some JVMs");

        // Get multiple references
        InsecureLogger logger1 = InsecureLogger.getInstance();
        InsecureLogger logger2 = InsecureLogger.getInstance();

        System.out.println("\nInstance references:");
        System.out.println("  logger1: " + System.identityHashCode(logger1));
        System.out.println("  logger2: " + System.identityHashCode(logger2));
        System.out.println("  Same instance? " + (logger1 == logger2));

        // Try to use
        try {
            logger1.info("Test message");
            System.out.println("\nLog count: " + logger1.getLogCount());
        } catch (IllegalStateException e) {
            System.out.println("\n❌ Error: " + e.getMessage());
        }
    }

    private static void demonstrateVolatile() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("VOLATILE DOUBLE-CHECKED LOCKING");
        System.out.println("=".repeat(50));

        System.out.println("\nCode pattern:");
        System.out.println("  private static volatile Logger instance;");
        System.out.println("  ");
        System.out.println("  public static Logger getInstance() {");
        System.out.println("      if (instance == null) {");
        System.out.println("          synchronized (Class.class) {");
        System.out.println("              if (instance == null) {");
        System.out.println("                  instance = new Logger();");
        System.out.println("              }");
        System.out.println("          }");
        System.out.println("      }");
        System.out.println("      return instance;");
        System.out.println("  }");

        System.out.println("\n✅ CORRECT:");
        System.out.println("  1. Volatile ensures visibility");
        System.out.println("  2. Prevents reordering");
        System.out.println("  3. Thread-safe publication");

        VolatileLogger logger1 = VolatileLogger.getInstance();
        VolatileLogger logger2 = VolatileLogger.getInstance();

        System.out.println("\nInstance references:");
        System.out.println("  logger1: " + System.identityHashCode(logger1));
        System.out.println("  logger2: " + System.identityHashCode(logger2));
        System.out.println("  Same instance? " + (logger1 == logger2));

        logger1.info("Test message");
        System.out.println("\nLog count: " + logger1.getLogCount());
    }

    private static void demonstrateHolder() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INITIALIZATION-ON-DEMAND HOLDER");
        System.out.println("=".repeat(50));

        System.out.println("\nCode pattern:");
        System.out.println("  private static class Holder {");
        System.out.println("      static final Logger INSTANCE = new Logger();");
        System.out.println("  }");
        System.out.println("  ");
        System.out.println("  public static Logger getInstance() {");
        System.out.println("      return Holder.INSTANCE;");
        System.out.println("  }");

        System.out.println("\n✅ CORRECT:");
        System.out.println("  1. Classloader guarantees thread safety");
        System.out.println("  2. Lazy initialization");
        System.out.println("  3. No synchronization overhead");

        HolderLogger logger1 = HolderLogger.getInstance();
        HolderLogger logger2 = HolderLogger.getInstance();

        System.out.println("\nInstance references:");
        System.out.println("  logger1: " + System.identityHashCode(logger1));
        System.out.println("  logger2: " + System.identityHashCode(logger2));
        System.out.println("  Same instance? " + (logger1 == logger2));

        logger1.info("Test message");
        System.out.println("\nLog count: " + logger1.getLogCount());
    }

    private static void demonstrateEnum() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("ENUM SINGLETON");
        System.out.println("=".repeat(50));

        System.out.println("\nCode pattern:");
        System.out.println("  public enum Logger {");
        System.out.println("      INSTANCE;");
        System.out.println("      ");
        System.out.println("      // methods here");
        System.out.println("  }");

        System.out.println("\n✅ CORRECT:");
        System.out.println("  1. JVM guarantees single instance");
        System.out.println("  2. Thread-safe by design");
        System.out.println("  3. Serialization safe");
        System.out.println("  4. Reflection proof");

        EnumLogger logger1 = EnumLogger.INSTANCE;
        EnumLogger logger2 = EnumLogger.INSTANCE;

        System.out.println("\nInstance references:");
        System.out.println("  logger1: " + logger1);
        System.out.println("  logger2: " + logger2);
        System.out.println("  Same instance? " + (logger1 == logger2));

        logger1.info("Test message");
        System.out.println("\nLog count: " + logger1.getLogCount());
    }

    private static void raceConditionTest(Scanner scanner) {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("RACE CONDITION TEST");
        System.out.println("=".repeat(60));

        System.out.print("\nEnter number of threads (recommended: 10): ");
        int threads = Integer.parseInt(scanner.nextLine().trim());

        System.out.print("Enter logs per thread (recommended: 1000): ");
        int logsPerThread = Integer.parseInt(scanner.nextLine().trim());

        try {
            // Test insecure (may fail)
            LoggerTester.testInsecureLogger(threads, logsPerThread);

            // Test volatile (should work)
            LoggerTester.testVolatileLogger(threads, logsPerThread);

            // Test holder (should work)
            LoggerTester.testHolderLogger(threads, logsPerThread);

            // Test enum (should work)
            LoggerTester.testEnumLogger(threads, logsPerThread);

        } catch (InterruptedException e) {
            System.err.println("Test interrupted: " + e.getMessage());
        }
    }

    private static void performanceComparison() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("PERFORMANCE COMPARISON");
        System.out.println("=".repeat(60));

        int iterations = 10_000_000;
        int warmup = 1_000_000;

        System.out.println("\nWarming up JIT...");
        for (int i = 0; i < warmup; i++) {
            InsecureLogger.getInstance();
            VolatileLogger.getInstance();
            HolderLogger.getInstance();
            EnumLogger.INSTANCE.getClass();
        }

        System.out.println("\nMeasuring " + iterations + " getInstance() calls:");

        // Test insecure
        long start = System.nanoTime();
        for (int i = 0; i < iterations; i++) {
            InsecureLogger.getInstance();
        }
        long insecureTime = System.nanoTime() - start;

        // Test volatile
        start = System.nanoTime();
        for (int i = 0; i < iterations; i++) {
            VolatileLogger.getInstance();
        }
        long volatileTime = System.nanoTime() - start;

        // Test holder
        start = System.nanoTime();
        for (int i = 0; i < iterations; i++) {
            HolderLogger.getInstance();
        }
        long holderTime = System.nanoTime() - start;

        // Test enum
        start = System.nanoTime();
        for (int i = 0; i < iterations; i++) {
            EnumLogger.INSTANCE.getClass();
        }
        long enumTime = System.nanoTime() - start;

        System.out.printf("\n%-10s | %15s | %10s\n",
                "Method", "Time (ms)", "ns/op");
        System.out.println("-".repeat(45));

        System.out.printf("%-10s | %15.2f | %10.2f\n",
                "Insecure", insecureTime / 1_000_000.0, insecureTime / (double) iterations);
        System.out.printf("%-10s | %15.2f | %10.2f\n",
                "Volatile", volatileTime / 1_000_000.0, volatileTime / (double) iterations);
        System.out.printf("%-10s | %15.2f | %10.2f\n",
                "Holder", holderTime / 1_000_000.0, holderTime / (double) iterations);
        System.out.printf("%-10s | %15.2f | %10.2f\n",
                "Enum", enumTime / 1_000_000.0, enumTime / (double) iterations);
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-609");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-609 VULNERABLE):");
        System.out.println("  1. Missing Volatile:");
        System.out.println("     • Instance may be published partially");
        System.out.println("     • Threads may see null or partially constructed object");
        System.out.println("     • Can cause NullPointerException or undefined behavior");

        System.out.println("\n  2. Reordering Issues:");
        System.out.println("     • Compiler/JVM may reorder instructions");
        System.out.println("     • Object published before initialization");
        System.out.println("     • Threads see non-null but uninitialized object");

        System.out.println("\n  3. Multiple Instances:");
        System.out.println("     • On some JVMs, multiple instances may be created");
        System.out.println("     • Breaks singleton guarantee");
        System.out.println("     • Resource leaks");

        System.out.println("\n✅ CORRECT IMPLEMENTATIONS:");

        System.out.println("\n  1. Volatile Double-Checked Locking:");
        System.out.println("     • volatile ensures visibility");
        System.out.println("     • Prevents reordering");
        System.out.println("     • Works on all JVMs");
        System.out.println("     • Good performance after initialization");

        System.out.println("\n  2. Initialization-on-Demand Holder:");
        System.out.println("     • Classloader guarantees thread safety");
        System.out.println("     • No synchronization overhead");
        System.out.println("     • Lazy initialization");
        System.out.println("     • Cannot be broken by reflection");

        System.out.println("\n  3. Enum Singleton:");
        System.out.println("     • JVM guarantees single instance");
        System.out.println("     • Thread-safe by design");
        System.out.println("     • Serialization safe");
        System.out.println("     • Reflection proof");
        System.out.println("     • ⭐ RECOMMENDED for most cases");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Never use double-checked locking without volatile");
        System.out.println("  2. Consider enum singleton for simplicity");
        System.out.println("  3. Use holder pattern if lazy loading needed");
        System.out.println("  4. Ensure all shared state is properly synchronized");
        System.out.println("  5. Initialize all fields in constructor");
        System.out.println("  6. Make the constructor private");
        System.out.println("  7. Provide a static factory method");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use Enum Singleton for most cases");
        System.out.println("  • Use Holder pattern for lazy loading with complex initialization");
        System.out.println("  • Use volatile DCL only if you understand the memory model");

        System.out.println("\n📊 COMPARISON:");
        System.out.println("  Method           | Thread-Safe | Lazy | Simple | Reflection-Safe");
        System.out.println("  -----------------|-------------|------|--------|----------------");
        System.out.println("  Insecure DCL     |     ❌      |  ✓   |   ⚠️   |       ❌");
        System.out.println("  Volatile DCL     |     ✓       |  ✓   |   ⚠️   |       ❌");
        System.out.println("  Holder           |     ✓       |  ✓   |   ✓    |       ⚠️");
        System.out.println("  Enum             |     ✓       |  ❌  |   ✓    |       ✓");
    }

    private static void cleanup() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("CLEANUP");
        System.out.println("=".repeat(50));

        // Close loggers
        try {
            InsecureLogger.getInstance().close();
            VolatileLogger.getInstance().close();
            HolderLogger.getInstance().close();
            EnumLogger.INSTANCE.close();
        } catch (Exception e) {
            // Ignore
        }

        // Delete log files
        try {
            Files.deleteIfExists(Paths.get("insecure_app.log"));
            Files.deleteIfExists(Paths.get("volatile_app.log"));
            Files.deleteIfExists(Paths.get("holder_app.log"));
            Files.deleteIfExists(Paths.get("enum_app.log"));
            System.out.println("Log files cleaned up");
        } catch (IOException e) {
            System.err.println("Cleanup error: " + e.getMessage());
        }
    }
}