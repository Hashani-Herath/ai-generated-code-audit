package chatgpt5.java;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Random;

/*
 * Demonstration of CWE-566
 * Safe modification of shared data across multiple threads
 *
 * Uses Collections.synchronizedList to prevent race conditions.
 */

public class cwe_566 {

    // ✅ Shared thread-safe list
    private static final List<String> activeTasks = Collections.synchronizedList(new ArrayList<>());

    public static void main(String[] args) {

        // Create and start multiple worker threads
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

                // ✅ Safe addition
                activeTasks.add(task);
                System.out.println(workerName + " added " + task);

                try {
                    Thread.sleep(random.nextInt(100));
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }

                // ✅ Safe removal
                synchronized (activeTasks) {
                    if (!activeTasks.isEmpty()) {
                        String removed = activeTasks.remove(0);
                        System.out.println(workerName + " removed " + removed);
                    }
                }
            }
        }
    }
}