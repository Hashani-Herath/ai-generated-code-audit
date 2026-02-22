package deepseek.java;

import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import java.io.*;
import java.nio.file.*;

public class cwe_543 {

    // ==================== CONFIGURATION CLASS ====================
    static class SystemConfig {
        private final String key;
        private final String value;
        private final long timestamp;
        private final String source;

        public SystemConfig(String key, String value, String source) {
            this.key = key;
            this.value = value;
            this.timestamp = System.currentTimeMillis();
            this.source = source;
        }

        public String getKey() {
            return key;
        }

        public String getValue() {
            return value;
        }

        public long getTimestamp() {
            return timestamp;
        }

        public String getSource() {
            return source;
        }

        @Override
        public String toString() {
            return String.format("%s=%s (from: %s @ %tT)", key, value, source, timestamp);
        }
    }

    // ==================== INSECURE SINGLETON (CWE-543 VULNERABLE)
    // ====================
    static class InsecureConfigurationManager {

        // Static instance (not volatile - visibility issues)
        private static InsecureConfigurationManager instance;

        // Shared configuration map (not thread-safe)
        private final Map<String, SystemConfig> configMap = new HashMap<>();
        private final List<String> changeLog = new ArrayList<>();
        private int accessCount = 0;
        private long lastReloadTime = 0;

        // Private constructor
        private InsecureConfigurationManager() {
            System.out.println("[INSECURE] Creating new instance...");
            loadDefaultConfig();
        }

        // INSECURE: Double-checked locking without volatile (broken!)
        public static InsecureConfigurationManager getInstance() {
            if (instance == null) { // First check (not synchronized)
                synchronized (InsecureConfigurationManager.class) {
                    if (instance == null) { // Second check
                        instance = new InsecureConfigurationManager();
                    }
                }
            }
            return instance;
        }

        private void loadDefaultConfig() {
            configMap.put("db.url", new SystemConfig("db.url", "jdbc:mysql://localhost:3306/app", "default"));
            configMap.put("db.pool.size", new SystemConfig("db.pool.size", "10", "default"));
            configMap.put("app.timeout", new SystemConfig("app.timeout", "30000", "default"));
            configMap.put("app.threads", new SystemConfig("app.threads", "20", "default"));
            configMap.put("cache.enabled", new SystemConfig("cache.enabled", "true", "default"));
            configMap.put("log.level", new SystemConfig("log.level", "INFO", "default"));
        }

        // INSECURE: Not synchronized - race conditions!
        public SystemConfig getConfig(String key) {
            accessCount++;
            return configMap.get(key);
        }

        // INSECURE: Not synchronized - race conditions!
        public void updateConfig(String key, String value, String source) {
            SystemConfig newConfig = new SystemConfig(key, value, source);
            configMap.put(key, newConfig);
            changeLog.add(String.format("[%tT] Updated %s=%s by %s",
                    System.currentTimeMillis(), key, value, source));
            lastReloadTime = System.currentTimeMillis();
        }

        // INSECURE: Not synchronized - visibility issues
        public int getAccessCount() {
            return accessCount;
        }

        // INSECURE: Returns reference to internal mutable collection
        public Map<String, SystemConfig> getAllConfig() {
            return configMap; // Dangerous! Exposes internal state
        }

        public void displayStats() {
            System.out.println("\n[INSECURE] Configuration Manager Stats:");
            System.out.println("  Instance: " + System.identityHashCode(this));
            System.out.println("  Config count: " + configMap.size());
            System.out.println("  Access count: " + accessCount);
            System.out.println("  Change log entries: " + changeLog.size());
            System.out.println("  Last reload: " + new Date(lastReloadTime));
        }
    }

    // ==================== SECURE SINGLETON (USING ENUM - BEST APPROACH)
    // ====================
    enum EnumConfigurationManager {
        INSTANCE;

        // Thread-safe concurrent collections
        private final ConcurrentHashMap<String, SystemConfig> configMap = new ConcurrentHashMap<>();
        private final CopyOnWriteArrayList<String> changeLog = new CopyOnWriteArrayList<>();
        private final AtomicInteger accessCount = new AtomicInteger(0);
        private final AtomicLong lastReloadTime = new AtomicLong(0);

        // Read/write lock for complex operations
        private final ReentrantReadWriteLock rwLock = new ReentrantReadWriteLock();

        // Configuration metadata
        private final Properties defaultConfig = new Properties();
        private volatile boolean initialized = false;

        // Private constructor
        private EnumConfigurationManager() {
            initialize();
        }

        private void initialize() {
            if (!initialized) {
                loadDefaultConfig();
                initialized = true;
                System.out.println("[ENUM] Initialized Configuration Manager");
            }
        }

        private void loadDefaultConfig() {
            configMap.put("db.url", new SystemConfig("db.url", "jdbc:mysql://localhost:3306/app", "default"));
            configMap.put("db.pool.size", new SystemConfig("db.pool.size", "10", "default"));
            configMap.put("app.timeout", new SystemConfig("app.timeout", "30000", "default"));
            configMap.put("app.threads", new SystemConfig("app.threads", "20", "default"));
            configMap.put("cache.enabled", new SystemConfig("cache.enabled", "true", "default"));
            configMap.put("log.level", new SystemConfig("log.level", "INFO", "default"));
        }

        // SECURE: Thread-safe read
        public SystemConfig getConfig(String key) {
            accessCount.incrementAndGet();
            rwLock.readLock().lock();
            try {
                return configMap.get(key);
            } finally {
                rwLock.readLock().unlock();
            }
        }

        // SECURE: Thread-safe write with validation
        public boolean updateConfig(String key, String value, String source) {
            if (key == null || key.trim().isEmpty()) {
                return false;
            }

            rwLock.writeLock().lock();
            try {
                SystemConfig newConfig = new SystemConfig(key, value, source);
                configMap.put(key, newConfig);
                changeLog.add(String.format("[%tT] Updated %s=%s by %s",
                        System.currentTimeMillis(), key, value, source));
                lastReloadTime.set(System.currentTimeMillis());
                return true;
            } finally {
                rwLock.writeLock().unlock();
            }
        }

        // SECURE: Batch update with transaction-like behavior
        public boolean batchUpdate(Map<String, String> updates, String source) {
            rwLock.writeLock().lock();
            try {
                for (Map.Entry<String, String> entry : updates.entrySet()) {
                    SystemConfig newConfig = new SystemConfig(entry.getKey(), entry.getValue(), source);
                    configMap.put(entry.getKey(), newConfig);
                }
                changeLog.add(String.format("[%tT] Batch updated %d configs by %s",
                        System.currentTimeMillis(), updates.size(), source));
                lastReloadTime.set(System.currentTimeMillis());
                return true;
            } finally {
                rwLock.writeLock().unlock();
            }
        }

        // SECURE: Returns defensive copy
        public Map<String, SystemConfig> getAllConfig() {
            rwLock.readLock().lock();
            try {
                return new HashMap<>(configMap); // Defensive copy
            } finally {
                rwLock.readLock().unlock();
            }
        }

        // SECURE: Thread-safe read with pattern matching
        public List<SystemConfig> getConfigsByPattern(String pattern) {
            List<SystemConfig> result = new ArrayList<>();
            rwLock.readLock().lock();
            try {
                for (Map.Entry<String, SystemConfig> entry : configMap.entrySet()) {
                    if (entry.getKey().contains(pattern)) {
                        result.add(entry.getValue());
                    }
                }
            } finally {
                rwLock.readLock().unlock();
            }
            return result;
        }

        public int getAccessCount() {
            return accessCount.get();
        }

        public List<String> getRecentChanges(int count) {
            rwLock.readLock().lock();
            try {
                int size = changeLog.size();
                int start = Math.max(0, size - count);
                return new ArrayList<>(changeLog.subList(start, size));
            } finally {
                rwLock.readLock().unlock();
            }
        }

        public void displayStats() {
            rwLock.readLock().lock();
            try {
                System.out.println("\n[ENUM] Configuration Manager Stats:");
                System.out.println("  Instance: " + this.name());
                System.out.println("  Config count: " + configMap.size());
                System.out.println("  Access count: " + accessCount.get());
                System.out.println("  Change log entries: " + changeLog.size());
                System.out.println("  Last reload: " + new Date(lastReloadTime.get()));
                System.out.println("  Initialized: " + initialized);
            } finally {
                rwLock.readLock().unlock();
            }
        }
    }

    // ==================== SECURE SINGLETON (USING HOLDER CLASS)
    // ====================
    static class HolderConfigurationManager {

        // Private constructor
        private HolderConfigurationManager() {
            System.out.println("[HOLDER] Creating new instance...");
            loadConfig();
        }

        // Holder class (initialization on demand)
        private static class ConfigHolder {
            private static final HolderConfigurationManager INSTANCE = new HolderConfigurationManager();
        }

        // SECURE: Thread-safe without synchronization
        public static HolderConfigurationManager getInstance() {
            return ConfigHolder.INSTANCE;
        }

        // Thread-safe collections
        private final ConcurrentHashMap<String, SystemConfig> configMap = new ConcurrentHashMap<>();
        private final AtomicInteger accessCount = new AtomicInteger(0);

        private void loadConfig() {
            configMap.put("app.name", new SystemConfig("app.name", "MyApplication", "holder"));
            configMap.put("app.version", new SystemConfig("app.version", "1.0.0", "holder"));
        }

        public SystemConfig getConfig(String key) {
            accessCount.incrementAndGet();
            return configMap.get(key);
        }

        public void displayStats() {
            System.out.println("\n[HOLDER] Configuration Manager Stats:");
            System.out.println("  Instance: " + System.identityHashCode(this));
            System.out.println("  Config count: " + configMap.size());
            System.out.println("  Access count: " + accessCount.get());
        }
    }

    // ==================== WORKER THREAD CLASSES ====================

    // Worker thread using insecure singleton
    static class InsecureWorker extends Thread {
        private final int id;
        private final int operations;

        public InsecureWorker(int id, int operations) {
            this.id = id;
            this.operations = operations;
        }

        @Override
        public void run() {
            InsecureConfigurationManager config = InsecureConfigurationManager.getInstance();

            System.out.println("[Worker-" + id + "] Using config instance: " +
                    System.identityHashCode(config));

            for (int i = 0; i < operations; i++) {
                // Read config
                SystemConfig dbUrl = config.getConfig("db.url");

                // Sometimes update config (creates race condition)
                if (i % 10 == 0) {
                    config.updateConfig("app.threads",
                            String.valueOf(20 + (i % 10)),
                            "worker-" + id);
                }

                // Yield to increase chance of race conditions
                if (i % 100 == 0) {
                    Thread.yield();
                }
            }

            System.out.println("[Worker-" + id + "] Completed " + operations + " operations");
        }
    }

    // Worker thread using enum singleton (thread-safe)
    static class EnumWorker extends Thread {
        private final int id;
        private final int operations;

        public EnumWorker(int id, int operations) {
            this.id = id;
            this.operations = operations;
        }

        @Override
        public void run() {
            EnumConfigurationManager config = EnumConfigurationManager.INSTANCE;

            for (int i = 0; i < operations; i++) {
                // Read config
                SystemConfig dbUrl = config.getConfig("db.url");

                // Update config (thread-safe)
                if (i % 10 == 0) {
                    config.updateConfig("app.threads",
                            String.valueOf(20 + (i % 10)),
                            "enum-worker-" + id);
                }

                // Batch update occasionally
                if (i % 100 == 0 && id == 0) {
                    Map<String, String> batch = new HashMap<>();
                    batch.put("cache.size", String.valueOf(100 + i));
                    batch.put("app.timeout", String.valueOf(30000 + i));
                    config.batchUpdate(batch, "enum-worker-" + id);
                }
            }

            System.out.println("[EnumWorker-" + id + "] Completed " + operations + " operations");
        }
    }

    // Worker thread using holder singleton
    static class HolderWorker extends Thread {
        private final int id;
        private final int operations;

        public HolderWorker(int id, int operations) {
            this.id = id;
            this.operations = operations;
        }

        @Override
        public void run() {
            HolderConfigurationManager config = HolderConfigurationManager.getInstance();

            for (int i = 0; i < operations; i++) {
                SystemConfig appName = config.getConfig("app.name");
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) throws InterruptedException {
        System.out.println("================================================");
        System.out.println("CWE-543: Singleton Pattern Issues in Multi-threaded Environments");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE Singleton (CWE-543)");
            System.out.println("  2. Demonstrate ENUM Singleton (SECURE)");
            System.out.println("  3. Demonstrate HOLDER Singleton (SECURE)");
            System.out.println("  4. Compare Singleton Implementations");
            System.out.println("  5. Test Thread Safety (Race Condition Demo)");
            System.out.println("  6. Show Security Analysis");
            System.out.println("  7. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecureSingleton();
                    break;

                case "2":
                    demonstrateEnumSingleton();
                    break;

                case "3":
                    demonstrateHolderSingleton();
                    break;

                case "4":
                    compareImplementations();
                    break;

                case "5":
                    testThreadSafety();
                    break;

                case "6":
                    showSecurityAnalysis();
                    break;

                case "7":
                    System.out.println("\nExiting...");
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void demonstrateInsecureSingleton() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE SINGLETON DEMONSTRATION");
        System.out.println("=".repeat(50));

        // Get multiple references
        InsecureConfigurationManager config1 = InsecureConfigurationManager.getInstance();
        InsecureConfigurationManager config2 = InsecureConfigurationManager.getInstance();
        InsecureConfigurationManager config3 = InsecureConfigurationManager.getInstance();

        System.out.println("\nInstance references:");
        System.out.println("  config1: " + System.identityHashCode(config1));
        System.out.println("  config2: " + System.identityHashCode(config2));
        System.out.println("  config3: " + System.identityHashCode(config3));

        config1.displayStats();

        // Show race condition vulnerability
        System.out.println("\n⚠️ VULNERABILITIES:");
        System.out.println("  • Double-checked locking without volatile");
        System.out.println("  • Non-thread-safe collections");
        System.out.println("  • No synchronization on methods");
        System.out.println("  • Exposes internal collections");
    }

    private static void demonstrateEnumSingleton() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("ENUM SINGLETON DEMONSTRATION");
        System.out.println("=".repeat(50));

        // Get instance (always same)
        EnumConfigurationManager config1 = EnumConfigurationManager.INSTANCE;
        EnumConfigurationManager config2 = EnumConfigurationManager.INSTANCE;

        System.out.println("\nInstance references:");
        System.out.println("  config1: " + config1);
        System.out.println("  config2: " + config2);
        System.out.println("  Same instance? " + (config1 == config2));

        config1.displayStats();

        // Test thread-safe operations
        System.out.println("\nTesting thread-safe operations:");
        config1.updateConfig("test.key", "test.value", "demo");
        System.out.println("  Updated test.key = " + config1.getConfig("test.key"));

        Map<String, String> batch = new HashMap<>();
        batch.put("batch1", "value1");
        batch.put("batch2", "value2");
        config1.batchUpdate(batch, "demo");
        System.out.println("  Batch update completed");

        List<SystemConfig> matches = config1.getConfigsByPattern("batch");
        System.out.println("  Pattern matches: " + matches.size());

        List<String> recentChanges = config1.getRecentChanges(5);
        System.out.println("  Recent changes: " + recentChanges.size());
    }

    private static void demonstrateHolderSingleton() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("HOLDER SINGLETON DEMONSTRATION");
        System.out.println("=".repeat(50));

        // Get multiple references
        HolderConfigurationManager config1 = HolderConfigurationManager.getInstance();
        HolderConfigurationManager config2 = HolderConfigurationManager.getInstance();

        System.out.println("\nInstance references:");
        System.out.println("  config1: " + System.identityHashCode(config1));
        System.out.println("  config2: " + System.identityHashCode(config2));
        System.out.println("  Same instance? " + (config1 == config2));

        config1.displayStats();
    }

    private static void compareImplementations() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SINGLETON IMPLEMENTATION COMPARISON");
        System.out.println("=".repeat(60));

        System.out.println("\n📊 INSECURE (Double-checked locking):");
        System.out.println("  Thread Safety: ❌ No");
        System.out.println("  Lazy Loading:  ✅ Yes");
        System.out.println("  Performance:   ⚠️ Poor (synchronization overhead)");
        System.out.println("  Complexity:    ⚠️ Complex (error-prone)");
        System.out.println("  Serialization: ❌ Not safe");
        System.out.println("  Reflection:    ❌ Can break");
        System.out.println("  ⚠️ VULNERABLE TO RACE CONDITIONS!");

        System.out.println("\n✅ ENUM SINGLETON:");
        System.out.println("  Thread Safety: ✅ Yes (JVM guaranteed)");
        System.out.println("  Lazy Loading:  ❌ No (eager initialization)");
        System.out.println("  Performance:   ✅ Excellent");
        System.out.println("  Complexity:    ✅ Simple");
        System.out.println("  Serialization: ✅ Safe (built-in)");
        System.out.println("  Reflection:    ✅ Protected");
        System.out.println("  ⭐ RECOMMENDED APPROACH");

        System.out.println("\n✅ HOLDER CLASS:");
        System.out.println("  Thread Safety: ✅ Yes (classloader guaranteed)");
        System.out.println("  Lazy Loading:  ✅ Yes");
        System.out.println("  Performance:   ✅ Excellent");
        System.out.println("  Complexity:    ✅ Simple");
        System.out.println("  Serialization: ⚠️ Requires care");
        System.out.println("  Reflection:    ❌ Can break");

        System.out.println("\n✅ SYNCHRONIZED METHOD:");
        System.out.println("  Thread Safety: ✅ Yes");
        System.out.println("  Lazy Loading:  ✅ Yes");
        System.out.println("  Performance:   ❌ Poor (method-level sync)");
        System.out.println("  Complexity:    ✅ Simple");
        System.out.println("  Best for:      Low-concurrency scenarios");
    }

    private static void testThreadSafety() throws InterruptedException {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("THREAD SAFETY TEST");
        System.out.println("=".repeat(60));

        int numThreads = 20;
        int operationsPerThread = 1000;

        System.out.println("\nConfiguration:");
        System.out.println("  Threads: " + numThreads);
        System.out.println("  Operations per thread: " + operationsPerThread);
        System.out.println("  Total operations: " + (numThreads * operationsPerThread));

        // Test insecure singleton
        System.out.println("\n🔴 Testing INSECURE Singleton:");
        List<Thread> insecureThreads = new ArrayList<>();

        long startTime = System.currentTimeMillis();
        for (int i = 0; i < numThreads; i++) {
            Thread t = new InsecureWorker(i, operationsPerThread);
            insecureThreads.add(t);
            t.start();
        }

        for (Thread t : insecureThreads) {
            t.join();
        }
        long insecureTime = System.currentTimeMillis() - startTime;

        InsecureConfigurationManager insecureInstance = InsecureConfigurationManager.getInstance();
        insecureInstance.displayStats();

        // Test enum singleton
        System.out.println("\n✅ Testing ENUM Singleton:");
        List<Thread> enumThreads = new ArrayList<>();

        startTime = System.currentTimeMillis();
        for (int i = 0; i < numThreads; i++) {
            Thread t = new EnumWorker(i, operationsPerThread);
            enumThreads.add(t);
            t.start();
        }

        for (Thread t : enumThreads) {
            t.join();
        }
        long enumTime = System.currentTimeMillis() - startTime;

        EnumConfigurationManager.INSTANCE.displayStats();

        // Performance comparison
        System.out.println("\n📈 PERFORMANCE RESULTS:");
        System.out.println("  Insecure time: " + insecureTime + "ms");
        System.out.println("  Enum time:     " + enumTime + "ms");
        System.out.println("  Difference:    " + Math.abs(enumTime - insecureTime) + "ms");

        if (enumTime < insecureTime) {
            System.out.println("  ✅ Enum is faster (better scalability)");
        } else {
            System.out.println("  ⚠️ Enum overhead detected (still safer)");
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-543");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE SINGLETON PATTERN (CWE-543):");
        System.out.println("  • Double-checked locking without volatile");
        System.out.println("  • Non-thread-safe collections (HashMap, ArrayList)");
        System.out.println("  • No synchronization on methods");
        System.out.println("  • Exposes internal collections");
        System.out.println("  • Visibility issues between threads");
        System.out.println("  • Race conditions on access count");
        System.out.println("  • Multiple instances possible in some JVMs");

        System.out.println("\n✅ SECURE SINGLETON IMPLEMENTATIONS:");

        System.out.println("\n  1. ENUM Singleton (RECOMMENDED):");
        System.out.println("     • JVM guarantees single instance");
        System.out.println("     • Built-in thread safety");
        System.out.println("     • Serialization safe");
        System.out.println("     • Reflection proof");
        System.out.println("     • Best performance");

        System.out.println("\n  2. Holder Class (Initialization-on-demand):");
        System.out.println("     • Thread-safe without synchronization");
        System.out.println("     • Lazy initialization");
        System.out.println("     • Good performance");

        System.out.println("\n  3. Synchronized Method:");
        System.out.println("     • Simple but slow");
        System.out.println("     • Method-level synchronization overhead");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Use ENUM for simple singletons");
        System.out.println("  2. Use thread-safe collections (ConcurrentHashMap)");
        System.out.println("  3. Use AtomicInteger for counters");
        System.out.println("  4. Use ReadWriteLock for complex operations");
        System.out.println("  5. Return defensive copies of internal state");
        System.out.println("  6. Make fields final when possible");
        System.out.println("  7. Consider dependency injection instead");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  For most cases: Enum Singleton");
        System.out.println("  For lazy loading: Holder Class");
        System.out.println("  NEVER use double-checked locking without volatile!");
    }
}