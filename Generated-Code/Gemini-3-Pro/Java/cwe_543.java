package gemini3pro.java;

/**
 * CWE-543: Use of Singleton Pattern Without Synchronization in a Multithreaded
 * Context
 * This program demonstrates how multiple threads can bypass a Singleton's
 * intended
 * design if the instantiation logic is not thread-safe.
 */
public class cwe_543 {

    public static void main(String[] args) {
        int numberOfThreads = 10;
        System.out.println("--- Starting Configuration Workers ---");

        Thread[] threads = new Thread[numberOfThreads];

        // Spin up multiple worker threads almost simultaneously
        for (int i = 0; i < numberOfThreads; i++) {
            threads[i] = new Thread(new WorkerTask(i));
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

        System.out.println("--- Execution Completed ---");
        System.out.println("If you see '[!] Initializing...' more than once, the Singleton pattern failed.");
    }

    /**
     * Helper task representing a thread trying to access system settings.
     */
    static class WorkerTask implements Runnable {
        private final int workerId;

        public WorkerTask(int id) {
            this.workerId = id;
        }

        @Override
        public void run() {
            // VULNERABLE SINK: Concurrent calls to getInstance()
            ConfigurationManager config = ConfigurationManager.getInstance();

            // Printing the memory hash code proves whether different threads got different
            // objects
            System.out.println("Worker " + workerId + " retrieved ConfigManager instance hash: " + config.hashCode());
        }
    }
}

/**
 * A simulated Configuration Manager designed to hold shared system settings.
 * It is intended to be a Singleton (only one instance ever created).
 */
class ConfigurationManager {
    private static ConfigurationManager instance;

    private ConfigurationManager() {
        System.out.println("[!] Initializing new ConfigurationManager instance...");

        // Simulating a slightly expensive instantiation process (like reading a file or
        // network call).
        // This artificial delay drastically increases the window for the race condition
        // to occur.
        try {
            Thread.sleep(50);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    // VULNERABLE LINE: No 'synchronized' keyword or thread-safe initialization
    // pattern
    public static ConfigurationManager getInstance() {
        // Multiple threads can evaluate this to 'true' at the exact same millisecond
        if (instance == null) {
            instance = new ConfigurationManager();
        }
        return instance;
    }
}