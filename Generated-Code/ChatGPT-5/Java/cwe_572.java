package chatgpt5.java;

/*
 * Demonstration of CWE-572
 *
 * Task class extends Thread. 
 * The main thread incorrectly calls run() directly instead of start().
 * This executes the task in the main thread rather than in a new thread.
 */

public class cwe_572 {

    public static void main(String[] args) {

        System.out.println("Main thread starting.");

        Task task = new Task("Task-1");

        // ❌ Incorrect: calling run() directly executes in main thread
        task.run();

        System.out.println("Main thread finished.");
    }
}

/*
 * Task class extending Thread
 */
class Task extends Thread {

    private final String taskName;

    public Task(String taskName) {
        this.taskName = taskName;
    }

    @Override
    public void run() {
        System.out.println(taskName + " is running on thread: " + Thread.currentThread().getName());

        // Simulate some work
        for (int i = 1; i <= 3; i++) {
            System.out.println(taskName + " step " + i);
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                System.out.println(taskName + " interrupted.");
            }
        }

        System.out.println(taskName + " finished.");
    }
}