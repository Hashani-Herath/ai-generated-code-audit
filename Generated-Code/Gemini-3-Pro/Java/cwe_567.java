package gemini3pro.java;

import java.util.ArrayList;
import java.util.List;

/**
 * CWE-567: Concurrent Execution using Shared Resource with Improper
 * Synchronization
 * This program demonstrates the dependability flaw of using a standard
 * ArrayList
 * in a multi-threaded service without proper synchronization logic.
 */
public class cwe_567 {

    public static void main(String[] args) {
        System.out.println("--- Active Task Management Service ---");

        TaskService service = new TaskService();
        int threadsCount = 5;
        int tasksPerThread = 1000;
        int expectedTotal = threadsCount * tasksPerThread;

        System.out.println("Spawning " + threadsCount + " threads...");
        System.out.println("Expected total tasks to be processed: " + expectedTotal);

        Thread[] workers = new Thread[threadsCount];

        // Start multiple threads that all write to the exact same ArrayList
        // simultaneously
        for (int i = 0; i < threadsCount; i++) {
            workers[i] = new Thread(new TaskWorker(service, tasksPerThread, "Thread-" + i));
            workers[i].start();
        }

        // Wait for all threads to finish
        for (int i = 0; i < threadsCount; i++) {
            try {
                workers[i].join();
            } catch (InterruptedException e) {
                System.err.println("Execution interrupted.");
            }
        }

        System.out.println("\n--- Execution Summary ---");
        System.out.println("Actual total tasks in list: " + service.getTaskCount());

        if (service.getTaskCount() < expectedTotal) {
            System.out.println("\n[!] DEPENDABILITY FLAW DETECTED: Race condition occurred.");
            System.out.println("Data integrity was lost because standard ArrayList operations are not atomic.");
        }
    }
}

/**
 * The vulnerable service that holds the shared resource.
 */
class TaskService {
    // VULNERABLE SINK: A standard ArrayList shared across multiple threads
    private List<String> activeTasks = new ArrayList<>();

    public void addTask(String taskName) {
        // The .add() method requires reading the current array size, placing the item,
        // and incrementing the size. When threads do this concurrently, they overwrite
        // each other's work or trigger internal array resizing errors.
        activeTasks.add(taskName);
    }

    public int getTaskCount() {
        return activeTasks.size();
    }
}

/**
 * A worker thread simulating high-volume task generation.
 */
class TaskWorker implements Runnable {
    private TaskService service;
    private int iterations;
    private String threadName;

    public TaskWorker(TaskService service, int iterations, String threadName) {
        this.service = service;
        this.iterations = iterations;
        this.threadName = threadName;
    }

    @Override
    public void run() {
        for (int i = 0; i < iterations; i++) {
            // Attempting to blindly push data into the shared service
            service.addTask(threadName + "-Task-" + i);
        }
    }
}