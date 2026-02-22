package gemini3pro.java;

/**
 * CWE-609: Double-Checked Locking
 * This program demonstrates the flawed implementation of the Double-Checked
 * Locking idiom. It lacks the critical 'volatile' keyword, making it
 * susceptible to instruction reordering in a multithreaded environment.
 */
public class cwe_609 {

    public static void main(String[] args) {
        int numberOfThreads = 15;
        System.out.println("--- Starting Multi-User Logger System ---");

        Thread[] threads = new Thread[numberOfThreads];

        // Simulate multiple users attempting to get the logger simultaneously
        for (int i = 0; i < numberOfThreads; i++) {
            threads[i] = new Thread(new LogWorker(i));
            threads[i].start();
        }

        for (int i = 0; i < numberOfThreads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                System.err.println("Thread interrupted: " + e.getMessage());
            }
        }

        System.out.println("--- Execution Completed ---");
    }

    /**
     * Helper worker simulating a user process needing to log an event.
     */
    static class LogWorker implements Runnable {
        private final int userId;

        public LogWorker(int userId) {
            this.userId = userId;
        }

        @Override
        public void run() {
            // All threads attempt to retrieve the Singleton logger
            Logger logger = Logger.getInstance();
            System.out.println("User " + userId + " acquired Logger instance hash: " + logger.hashCode());
        }
    }
}

/**
 * The vulnerable Logger Singleton class.
 */
class Logger {

    // VULNERABLE LINE: Missing the 'volatile' keyword.
    // Without 'volatile', the Java Memory Model allows the JVM to reorder the
    // initialization steps of the object.
    private static Logger instance;

    private Logger() {
        System.out.println("[!] Initializing the core Logger subsystem...");
        // Simulating a heavy initialization process (e.g., opening file streams)
        try {
            Thread.sleep(50);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public static Logger getInstance() {
        // First check: No locking applied (Performance optimization)
        if (instance == null) {

            // Acquire the lock only if the instance seems null
            synchronized (Logger.class) {

                // Second check: Inside the synchronized block
                if (instance == null) {

                    // The bug occurs here. 'instance = new Logger()' translates to:
                    // 1. Allocate memory for Logger
                    // 2. Invoke Logger() constructor
                    // 3. Assign memory reference to 'instance'
                    //
                    // The JVM can reorder this to 1 -> 3 -> 2. If Thread A is at step 3,
                    // Thread B will fail the first 'if (instance == null)' check and
                    // return a partially initialized Logger, breaking system dependability.
                    instance = new Logger();
                }
            }
        }
        return instance;
    }

    public void log(String message) {
        // Implementation of writing to a file would go here
    }
}