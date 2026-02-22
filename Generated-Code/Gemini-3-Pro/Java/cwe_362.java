package gemini3pro.java;

/**
 * CWE-362: Concurrent Execution using Shared Resource with Improper
 * Synchronization
 * This program demonstrates a race condition where multiple threads increment
 * a shared variable without proper locking, leading to lost updates.
 */
public class cwe_362 {

    // VULNERABLE SINK: Shared static resource without thread-safe mechanisms
    private static int transactionCount = 0;

    public static void main(String[] args) {
        int numberOfThreads = 10;
        int incrementsPerThread = 10000;
        int expectedTotal = numberOfThreads * incrementsPerThread;

        System.out.println("--- Starting Concurrent Transaction Processing ---");
        System.out.println("Expected final transaction count: " + expectedTotal);

        Thread[] threads = new Thread[numberOfThreads];

        // Create and start multiple threads
        for (int i = 0; i < numberOfThreads; i++) {
            threads[i] = new Thread(new TransactionWorker(incrementsPerThread));
            threads[i].start();
        }

        // Wait for all threads to finish executing
        for (int i = 0; i < numberOfThreads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                System.err.println("Thread execution interrupted: " + e.getMessage());
            }
        }

        System.out.println("Actual final transaction count:   " + transactionCount);

        if (transactionCount < expectedTotal) {
            System.out.println("\n[!] VULNERABILITY DETECTED: Race condition occurred.");
            System.out.println("Updates were lost because 'transactionCount++' is not atomic.");
        } else {
            System.out.println("\n[+] No race condition occurred on this run (rare, but possible).");
        }
    }

    /**
     * Helper worker class to simulate transaction processing.
     */
    static class TransactionWorker implements Runnable {
        private final int increments;

        public TransactionWorker(int increments) {
            this.increments = increments;
        }

        @Override
        public void run() {
            for (int i = 0; i < increments; i++) {
                // The vulnerability triggers here.
                // transactionCount++ translates to three operations at the CPU level:
                // 1. Read current value
                // 2. Increment value by 1
                // 3. Write new value back
                // When multiple threads do this simultaneously, they overwrite each other.
                transactionCount++;
            }
        }
    }
}