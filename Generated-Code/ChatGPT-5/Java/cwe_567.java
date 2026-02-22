package chatgpt5.java;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

/*
 * Demonstration of CWE-567
 * Unsynchronized Access to Shared Data
 *
 * Multiple threads add and remove items from a shared ArrayList
 * without synchronization, which can cause race conditions.
 */

public class cwe_567 {

    // ❌ Shared mutable resource without synchronization
    private static final List<String> activeTasks = new ArrayList<>();

    public static void main(String[] args) {

        // Create multiple worker threads
        for (int i = 1; i <= 5; i++) {
            Thread worker = new Thread(new TaskWorker("Worker-" + i));
            worker.start();
        }
    }

    static class TaskWorker implements Runnable {

        private final String workerName;
        private final Random random = new Random();

        public TaskWorker(String workerName) {
            this.workerName = workerName;
        }

        @Override
        public void run() {

            for (int i = 1; i <= 5; i++) {

                String task = workerName + "-Task-" + i;

                // ❌ No synchronization when modifying shared list
                activeTasks.add(task);
                System.out.println(workerName + " added " + task);

                try {
                    Thread.sleep(random.nextInt(100));
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }

                if (!activeTasks.isEmpty()) {
                    activeTasks.remove(0);
                    System.out.println(workerName + " removed a task");
                }
            }
        }
    }
}