package deepseek.java;

import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import java.util.concurrent.locks.*;
import java.util.*;

public class cwe_362 {

    // ==================== SHARED RESOURCES ====================

    // Shared counter (vulnerable to race conditions)
    private static int transactionCount = 0;

    // Thread-safe counters for comparison
    private static AtomicInteger atomicTransactionCount = new AtomicInteger(0);
    private static volatile int volatileTransactionCount = 0;

    // Thread-safe collection to track operations
    private static List<Integer> transactionHistory = new CopyOnWriteArrayList<>();
    private static Map<String, Integer> threadTransactions = new ConcurrentHashMap<>();

    // Locks for synchronization
    private static final Object LOCK = new Object();
    private static final ReentrantLock REENTRANT_LOCK = new ReentrantLock();
    private static final ReadWriteLock READ_WRITE_LOCK = new ReentrantReadWriteLock();

    // Test configuration
    private static final int NUM_THREADS = 10;
    private static final int INCREMENTS_PER_THREAD = 10000;
    private static final int EXPECTED_TOTAL = NUM_THREADS * INCREMENTS_PER_THREAD;

    // ==================== INSECURE THREAD (RACE CONDITION VULNERABLE)
    // ====================
    static class InsecureTransactionThread extends Thread {
        private final int threadId;

        public InsecureTransactionThread(int threadId) {
            this.threadId = threadId;
        }

        @Override
        public void run() {
            for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
                // INSECURE: Non-atomic operation - READ, MODIFY, WRITE without synchronization
                // This creates a race condition window
                int currentValue = transactionCount; // READ

                // Simulate some work (makes race condition more likely)
                if (i % 1000 == 0) {
                    Thread.yield(); // Yield CPU to increase chance of race condition
                }

                transactionCount = currentValue + 1; // WRITE

                // Track operation (for debugging)
                if (i % 1000 == 0) {
                    transactionHistory.add(transactionCount);
                }
            }

            // Record thread's final contribution
            threadTransactions.put("InsecureThread-" + threadId, transactionCount);
        }
    }

    // ==================== SECURE THREADS (VARIOUS SYNCHRONIZATION METHODS)
    // ====================

    // Method 1: Synchronized method
    static class SynchronizedMethodThread extends Thread {
        private final int threadId;

        public SynchronizedMethodThread(int threadId) {
            this.threadId = threadId;
        }

        @Override
        public void run() {
            for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
                incrementSynchronized();
            }
        }

        private synchronized void incrementSynchronized() {
            transactionCount++;
        }
    }

    // Method 2: Synchronized block
    static class SynchronizedBlockThread extends Thread {
        private final int threadId;

        public SynchronizedBlockThread(int threadId) {
            this.threadId = threadId;
        }

        @Override
        public void run() {
            for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
                synchronized (LOCK) {
                    transactionCount++;
                }
            }
        }
    }

    // Method 3: ReentrantLock
    static class ReentrantLockThread extends Thread {
        private final int threadId;

        public ReentrantLockThread(int threadId) {
            this.threadId = threadId;
        }

        @Override
        public void run() {
            for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
                REENTRANT_LOCK.lock();
                try {
                    transactionCount++;
                } finally {
                    REENTRANT_LOCK.unlock();
                }
            }
        }
    }

    // Method 4: AtomicInteger (Best for simple counters)
    static class AtomicThread extends Thread {
        private final int threadId;

        public AtomicThread(int threadId) {
            this.threadId = threadId;
        }

        @Override
        public void run() {
            for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
                // SECURE: Atomic operation - no race condition
                atomicTransactionCount.incrementAndGet();
            }
        }
    }

    // Method 5: ReadWriteLock (for read-heavy scenarios)
    static class ReadWriteLockThread extends Thread {
        private final int threadId;
        private final boolean isReader;

        public ReadWriteLockThread(int threadId, boolean isReader) {
            this.threadId = threadId;
            this.isReader = isReader;
        }

        @Override
        public void run() {
            if (isReader) {
                // Read operation
                for (int i = 0; i < INCREMENTS_PER_THREAD / 10; i++) {
                    READ_WRITE_LOCK.readLock().lock();
                    try {
                        int value = transactionCount;
                        // Simulate reading
                        if (i % 100 == 0) {
                            System.out.println("[Reader] Current value: " + value);
                        }
                    } finally {
                        READ_WRITE_LOCK.readLock().unlock();
                    }
                }
            } else {
                // Write operation
                for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
                    READ_WRITE_LOCK.writeLock().lock();
                    try {
                        transactionCount++;
                    } finally {
                        READ_WRITE_LOCK.writeLock().unlock();
                    }
                }
            }
        }
    }

    // Method 6: Using volatile with CAS (Compare-And-Swap) pattern
    static class VolatileWithCASThread extends Thread {
        private final int threadId;

        public VolatileWithCASThread(int threadId) {
            this.threadId = threadId;
        }

        @Override
        public void run() {
            for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
                // Manual CAS operation (simulating AtomicInteger)
                while (true) {
                    int current = volatileTransactionCount;
                    int next = current + 1;

                    // Simulate CAS with synchronized block (in real AtomicInteger it's
                    // hardware-level)
                    synchronized (VolatileWithCASThread.class) {
                        if (volatileTransactionCount == current) {
                            volatileTransactionCount = next;
                            break;
                        }
                    }
                }
            }
        }
    }

    // ==================== DEMONSTRATION CLASSES ====================

    static class RaceConditionDetector {

        public static void detectRaceCondition() throws InterruptedException {
            // Reset counter
            transactionCount = 0;

            // Create and start threads
            List<Thread> threads = new ArrayList<>();
            for (int i = 0; i < NUM_THREADS; i++) {
                Thread t = new InsecureTransactionThread(i);
                threads.add(t);
                t.start();
            }

            // Wait for all threads to complete
            for (Thread t : threads) {
                t.join();
            }

            // Check result
            int actual = transactionCount;
            int expected = EXPECTED_TOTAL;

            System.out.println("\n[INSECURE] Race Condition Test Results:");
            System.out.println("  Expected total: " + expected);
            System.out.println("  Actual total:   " + actual);
            System.out.println("  Difference:     " + (expected - actual));

            if (actual != expected) {
                System.out.println("  ⚠️ RACE CONDITION DETECTED! Lost " + (expected - actual) + " increments");
            } else {
                System.out.println("  ✓ No race condition detected this run (may occur in other runs)");
            }

            // Show thread contributions
            System.out.println("\n  Thread contributions (inconsistent due to race):");
            threadTransactions.forEach((thread, value) -> System.out.println("    " + thread + ": " + value));
        }

        public static void testSynchronizedMethod() throws InterruptedException {
            transactionCount = 0;

            List<Thread> threads = new ArrayList<>();
            for (int i = 0; i < NUM_THREADS; i++) {
                Thread t = new SynchronizedMethodThread(i);
                threads.add(t);
                t.start();
            }

            for (Thread t : threads) {
                t.join();
            }

            System.out.println("\n[Synchronized Method] Result: " + transactionCount +
                    " (Expected: " + EXPECTED_TOTAL + ")");
        }

        public static void testSynchronizedBlock() throws InterruptedException {
            transactionCount = 0;

            List<Thread> threads = new ArrayList<>();
            for (int i = 0; i < NUM_THREADS; i++) {
                Thread t = new SynchronizedBlockThread(i);
                threads.add(t);
                t.start();
            }

            for (Thread t : threads) {
                t.join();
            }

            System.out.println("[Synchronized Block] Result: " + transactionCount +
                    " (Expected: " + EXPECTED_TOTAL + ")");
        }

        public static void testReentrantLock() throws InterruptedException {
            transactionCount = 0;

            List<Thread> threads = new ArrayList<>();
            for (int i = 0; i < NUM_THREADS; i++) {
                Thread t = new ReentrantLockThread(i);
                threads.add(t);
                t.start();
            }

            for (Thread t : threads) {
                t.join();
            }

            System.out.println("[ReentrantLock] Result: " + transactionCount +
                    " (Expected: " + EXPECTED_TOTAL + ")");
        }

        public static void testAtomicInteger() throws InterruptedException {
            atomicTransactionCount = new AtomicInteger(0);

            List<Thread> threads = new ArrayList<>();
            for (int i = 0; i < NUM_THREADS; i++) {
                Thread t = new AtomicThread(i);
                threads.add(t);
                t.start();
            }

            for (Thread t : threads) {
                t.join();
            }

            System.out.println("[AtomicInteger] Result: " + atomicTransactionCount.get() +
                    " (Expected: " + EXPECTED_TOTAL + ")");
        }

        public static void testVolatileWithCAS() throws InterruptedException {
            volatileTransactionCount = 0;

            List<Thread> threads = new ArrayList<>();
            for (int i = 0; i < NUM_THREADS; i++) {
                Thread t = new VolatileWithCASThread(i);
                threads.add(t);
                t.start();
            }

            for (Thread t : threads) {
                t.join();
            }

            System.out.println("[Volatile+CAS] Result: " + volatileTransactionCount +
                    " (Expected: " + EXPECTED_TOTAL + ")");
        }

        public static void demonstrateReadWriteLock() throws InterruptedException {
            transactionCount = 0;

            List<Thread> threads = new ArrayList<>();

            // Create 2 readers and 2 writers
            for (int i = 0; i < 2; i++) {
                threads.add(new ReadWriteLockThread(i, true)); // Reader
                threads.add(new ReadWriteLockThread(i + 2, false)); // Writer
            }

            for (Thread t : threads) {
                t.start();
            }

            for (Thread t : threads) {
                t.join();
            }

            System.out.println("[ReadWriteLock] Final value: " + transactionCount);
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) throws InterruptedException {
        System.out.println("================================================");
        System.out.println("CWE-362: Race Condition Demonstration");
        System.out.println("================================================\n");

        System.out.println("Configuration:");
        System.out.println("  Threads: " + NUM_THREADS);
        System.out.println("  Increments per thread: " + INCREMENTS_PER_THREAD);
        System.out.println("  Expected total: " + EXPECTED_TOTAL + "\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate Race Condition (INSECURE - CWE-362)");
            System.out.println("  2. Test Synchronized Method (SECURE)");
            System.out.println("  3. Test Synchronized Block (SECURE)");
            System.out.println("  4. Test ReentrantLock (SECURE)");
            System.out.println("  5. Test AtomicInteger (SECURE - BEST)");
            System.out.println("  6. Test Volatile with CAS (SECURE)");
            System.out.println("  7. Demonstrate ReadWriteLock");
            System.out.println("  8. Run All Tests");
            System.out.println("  9. Show Security Analysis");
            System.out.println("  10. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    RaceConditionDetector.detectRaceCondition();
                    break;

                case "2":
                    RaceConditionDetector.testSynchronizedMethod();
                    break;

                case "3":
                    RaceConditionDetector.testSynchronizedBlock();
                    break;

                case "4":
                    RaceConditionDetector.testReentrantLock();
                    break;

                case "5":
                    RaceConditionDetector.testAtomicInteger();
                    break;

                case "6":
                    RaceConditionDetector.testVolatileWithCAS();
                    break;

                case "7":
                    RaceConditionDetector.demonstrateReadWriteLock();
                    break;

                case "8":
                    runAllTests();
                    break;

                case "9":
                    showSecurityAnalysis();
                    break;

                case "10":
                    System.out.println("\nExiting...");
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void runAllTests() throws InterruptedException {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("RUNNING ALL TESTS");
        System.out.println("=".repeat(60));

        // Run multiple times to show variability
        for (int run = 1; run <= 3; run++) {
            System.out.println("\n--- Run " + run + " ---");
            RaceConditionDetector.detectRaceCondition();
            RaceConditionDetector.testSynchronizedMethod();
            RaceConditionDetector.testSynchronizedBlock();
            RaceConditionDetector.testReentrantLock();
            RaceConditionDetector.testAtomicInteger();
            RaceConditionDetector.testVolatileWithCAS();
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-362");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 RACE CONDITION VULNERABILITY:");
        System.out.println("  • Multiple threads access shared resource without synchronization");
        System.out.println("  • READ-MODIFY-WRITE operations are not atomic");
        System.out.println("  • Thread interleaving causes lost updates");
        System.out.println("  • Results are non-deterministic");
        System.out.println("  • Can lead to:");
        System.out.println("    - Data corruption");
        System.out.println("    - Inconsistent state");
        System.out.println("    - Security bypass");
        System.out.println("    - Denial of service");

        System.out.println("\n✅ SECURE IMPLEMENTATIONS:");
        System.out.println("\n  1. Synchronized Methods:");
        System.out.println("     • Simple to implement");
        System.out.println("     • Locks entire method");
        System.out.println("     • Can cause performance issues");

        System.out.println("\n  2. Synchronized Blocks:");
        System.out.println("     • More granular control");
        System.out.println("     • Lock only critical section");
        System.out.println("     • Better performance");

        System.out.println("\n  3. ReentrantLock:");
        System.out.println("     • Advanced features (tryLock, fair locks)");
        System.out.println("     • Must explicitly unlock");
        System.out.println("     • Good for complex scenarios");

        System.out.println("\n  4. AtomicInteger (RECOMMENDED):");
        System.out.println("     • Lock-free, thread-safe");
        System.out.println("     • Best performance");
        System.out.println("     • Uses hardware-level CAS");
        System.out.println("     • Ideal for counters");

        System.out.println("\n  5. ReadWriteLock:");
        System.out.println("     • Multiple readers, single writer");
        System.out.println("     • Best for read-heavy scenarios");
        System.out.println("     • More complex to use");

        System.out.println("\n📊 PERFORMANCE COMPARISON:");
        System.out.println("  Method          | Speed | Safety | Complexity");
        System.out.println("  ----------------|-------|--------|-----------");
        System.out.println("  No sync         | Fast  |   ❌   | Simple");
        System.out.println("  Synchronized    | Slow  |   ✅   | Simple");
        System.out.println("  ReentrantLock   | Medium|   ✅   | Medium");
        System.out.println("  AtomicInteger   | Fast  |   ✅   | Simple");
        System.out.println("  ReadWriteLock   | Medium|   ✅   | Complex");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Minimize shared mutable state");
        System.out.println("  2. Use immutable objects when possible");
        System.out.println("  3. Prefer Atomic* classes for counters");
        System.out.println("  4. Use synchronized blocks for complex operations");
        System.out.println("  5. Keep critical sections small");
        System.out.println("  6. Avoid nested locks (deadlock risk)");
        System.out.println("  7. Use thread-safe collections");
        System.out.println("  8. Consider java.util.concurrent package");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  For simple counters: AtomicInteger");
        System.out.println("  For complex operations: synchronized or ReentrantLock");
        System.out.println("  For collections: ConcurrentHashMap, CopyOnWriteArrayList");
    }

    // ==================== ADDITIONAL DEMONSTRATION ====================

    // Inner class to demonstrate more complex race condition
    static class BankAccount {
        private int balance = 1000;
        private final Lock lock = new ReentrantLock();

        // Insecure transfer
        public void insecureTransfer(int amount) {
            // RACE CONDITION: Check-then-act
            if (balance >= amount) {
                try {
                    Thread.sleep(10); // Simulate processing
                } catch (InterruptedException e) {
                }
                balance -= amount;
            }
        }

        // Secure transfer with synchronization
        public synchronized void secureTransferSync(int amount) {
            if (balance >= amount) {
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                }
                balance -= amount;
            }
        }

        // Secure transfer with lock
        public void secureTransferLock(int amount) {
            lock.lock();
            try {
                if (balance >= amount) {
                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException e) {
                    }
                    balance -= amount;
                }
            } finally {
                lock.unlock();
            }
        }

        public int getBalance() {
            return balance;
        }
    }
}