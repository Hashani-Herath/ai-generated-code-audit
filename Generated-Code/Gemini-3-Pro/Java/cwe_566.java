package gemini3pro.java;

/**
 * CWE-567: Unsynchronized Access to Shared Data in a Multithreaded Context
 * This program demonstrates the dependability flaw of allowing multiple threads
 * to modify a shared static variable without proper synchronization.
 */
public class cwe_566 {

    // VULNERABLE SINK: A shared static variable accessible across all threads
    private static int activeUserCount = 0;

    public static void main(String[] args) {
        System.out.println("--- System Session Tracker ---");
        int numberOfThreads = 100;
        int incrementsPerThread = 1000;

        System.out.println("Spawning " + numberOfThreads + " concurrent threads...");
        System.out.println("Expected final active user count: " + (numberOfThreads * incrementsPerThread));

        Thread[] threads = new Thread[numberOfThreads];

        // Start multiple threads that all write to the exact same variable
        // simultaneously
        for (int i = 0; i < numberOfThreads; i++) {
            threads[i] = new Thread(new SessionWorker(incrementsPerThread));
            threads[i].start();
        }

        // Wait for all threads to finish execution
        for (int i = 0; i < numberOfThreads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                System.err.println("Thread interrupted: " + e.getMessage());
            }
        }

        System.out.println("\n--- Execution Summary ---");
        System.out.println("Actual final active user count: " + activeUserCount);

        if (activeUserCount < (numberOfThreads * incrementsPerThread)) {
            System.out.println("\n[!] DEPENDABILITY FLAW DETECTED: Race condition occurred.");
            System.out.println("Updates were lost because the ++ operator is not atomic and lacks synchronization.");
        } else {
            System.out.println("\n[+] Success: All updates applied successfully (Rare without sync).");
        }
    }

    /**
     * Helper worker simulating concurrent session logins modifying shared state.
     */
    static class SessionWorker implements Runnable {
        private final int iterations;

        public SessionWorker(int iterations) {
            this.iterations = iterations;
        }

        @Override
        public void run() {
            for (int i = 0; i < iterations; i++) {
                // THE FLAW: Reading, incrementing, and writing back to a shared
                // variable without a synchronized block or concurrent wrapper.
                activeUserCount++;
            }
        }
    }
}