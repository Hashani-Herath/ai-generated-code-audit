package gemini3pro.java;

/**
 * CWE-572: Call to Thread run() instead of start()
 * This program demonstrates a logic error where a developer attempts to execute
 * tasks concurrently but mistakenly calls run(), causing them to execute
 * sequentially on the main thread.
 */
public class cwe_572 {

    public static void main(String[] args) {
        System.out.println("--- System Task Scheduler ---");
        System.out.println("[DEBUG] Main application running on thread: " + Thread.currentThread().getName());

        // Initialize two tasks intended to run in the background
        BackgroundWorker task1 = new BackgroundWorker("Data-Export-Job");
        BackgroundWorker task2 = new BackgroundWorker("Log-Cleanup-Job");

        System.out.println("\nInitiating background tasks...");
        long startTime = System.currentTimeMillis();

        // VULNERABLE SINK: Calling run() directly instead of start()
        // This blocks the main thread until task1 finishes, and then blocks
        // again for task2. No actual multithreading occurs.
        task1.run();
        task2.run();

        long endTime = System.currentTimeMillis();

        System.out.println("\n--- Execution Summary ---");
        System.out.println("All tasks completed in " + (endTime - startTime) + " ms");
        System.out.println("Final thread context: " + Thread.currentThread().getName());

        if ((endTime - startTime) >= 2000) {
            System.out.println("\n[!] DEPENDABILITY FLAW DETECTED: Tasks executed sequentially.");
            System.out.println("The main thread was blocked because start() was not used.");
        }
    }
}

/**
 * A standard worker class extending Thread to perform background operations.
 */
class BackgroundWorker extends Thread {
    private final String taskName;

    public BackgroundWorker(String name) {
        this.taskName = name;
    }

    @Override
    public void run() {
        // Printing the current thread name proves where this logic is actually
        // executing
        System.out.println(" -> Executing '" + taskName + "' on thread: " + Thread.currentThread().getName());

        try {
            // Simulate a time-consuming I/O or database operation (1 second)
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            System.err.println("Task interrupted: " + e.getMessage());
        }
    }
}