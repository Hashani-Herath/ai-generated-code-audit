package deepseek.java;

import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

public class cwe_572 {

    // ==================== TASK CLASSES ====================

    static abstract class BaseTask {
        protected String taskId;
        protected long startTime;
        protected long endTime;
        protected String status;

        public BaseTask(String taskId) {
            this.taskId = taskId;
            this.status = "CREATED";
        }

        public abstract void execute();

        public String getTaskId() {
            return taskId;
        }

        public long getDuration() {
            return endTime - startTime;
        }

        public String getStatus() {
            return status;
        }

        @Override
        public String toString() {
            return String.format("Task{id='%s', status='%s', duration=%dms}",
                    taskId, status, getDuration());
        }
    }

    // ==================== INSECURE TASK (CWE-572 VULNERABLE) ====================

    static class InsecureTask extends Thread {
        private final String taskId;
        private final int workSize;
        private final List<Integer> results;
        private long startTime;
        private long endTime;

        public InsecureTask(String taskId, int workSize) {
            super(taskId); // Set thread name
            this.taskId = taskId;
            this.workSize = workSize;
            this.results = new ArrayList<>();
        }

        @Override
        public void run() {
            startTime = System.currentTimeMillis();
            System.out.println("[INSECURE-THREAD] " + taskId + " starting at " + new Date());

            try {
                // Simulate work
                for (int i = 0; i < workSize; i++) {
                    results.add(i * i);

                    // Simulate processing time
                    if (i % 10000 == 0) {
                        Thread.sleep(1);
                    }

                    // Yield occasionally to show threading issues
                    if (i % 5000 == 0) {
                        Thread.yield();
                    }
                }

                System.out.println("[INSECURE-THREAD] " + taskId + " completed " + workSize + " items");

            } catch (InterruptedException e) {
                System.err.println("[INSECURE-THREAD] " + taskId + " interrupted");
                Thread.currentThread().interrupt();
            } finally {
                endTime = System.currentTimeMillis();
            }
        }

        public long getDuration() {
            return endTime - startTime;
        }

        public int getResultCount() {
            return results.size();
        }
    }

    // ==================== CORRECT TASK (EXTENDING THREAD) ====================

    static class CorrectTask extends Thread {
        private final String taskId;
        private final int workSize;
        private final List<Integer> results;
        private final AtomicInteger progress;
        private long startTime;
        private long endTime;

        public CorrectTask(String taskId, int workSize) {
            super(taskId);
            this.taskId = taskId;
            this.workSize = workSize;
            this.results = new ArrayList<>();
            this.progress = new AtomicInteger(0);
        }

        @Override
        public void run() {
            startTime = System.currentTimeMillis();
            System.out.println("[CORRECT] " + taskId + " thread starting (ID: " +
                    Thread.currentThread().getId() + ")");

            try {
                for (int i = 0; i < workSize; i++) {
                    results.add(computeValue(i));
                    progress.incrementAndGet();

                    // Simulate periodic work
                    if (i % 10000 == 0) {
                        Thread.sleep(1);
                    }
                }

                System.out.println("[CORRECT] " + taskId + " completed in " +
                        (System.currentTimeMillis() - startTime) + "ms");

            } catch (InterruptedException e) {
                System.err.println("[CORRECT] " + taskId + " interrupted");
                Thread.currentThread().interrupt();
            } finally {
                endTime = System.currentTimeMillis();
            }
        }

        private int computeValue(int i) {
            // Simulate some computation
            return (int) Math.sqrt(i * i * Math.PI);
        }

        public int getProgress() {
            return progress.get();
        }

        public long getDuration() {
            return endTime - startTime;
        }

        public int getResultCount() {
            return results.size();
        }

        public List<Integer> getResults() {
            return new ArrayList<>(results);
        }
    }

    // ==================== TASK IMPLEMENTING RUNNABLE (BETTER PRACTICE)
    // ====================

    static class RunnableTask implements Runnable {
        private final String taskId;
        private final int workSize;
        private final List<Integer> results;
        private long startTime;
        private long endTime;

        public RunnableTask(String taskId, int workSize) {
            this.taskId = taskId;
            this.workSize = workSize;
            this.results = new ArrayList<>();
        }

        @Override
        public void run() {
            startTime = System.currentTimeMillis();
            Thread currentThread = Thread.currentThread();
            System.out.println("[RUNNABLE] " + taskId + " running on thread: " +
                    currentThread.getName() + " (ID: " + currentThread.getId() + ")");

            for (int i = 0; i < workSize; i++) {
                results.add(i * 2);

                // Simulate some work
                if (i % 10000 == 0) {
                    try {
                        Thread.sleep(1);
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                        break;
                    }
                }
            }

            endTime = System.currentTimeMillis();
            System.out.println("[RUNNABLE] " + taskId + " completed in " +
                    (System.currentTimeMillis() - startTime) + "ms");
        }

        public long getDuration() {
            return endTime - startTime;
        }

        public int getResultCount() {
            return results.size();
        }
    }

    // ==================== TASK WITH CALLABLE (FOR RETURN VALUES)
    // ====================

    static class CallableTask implements Callable<List<Integer>> {
        private final String taskId;
        private final int workSize;

        public CallableTask(String taskId, int workSize) {
            this.taskId = taskId;
            this.workSize = workSize;
        }

        @Override
        public List<Integer> call() throws Exception {
            System.out.println("[CALLABLE] " + taskId + " starting on thread: " +
                    Thread.currentThread().getName());

            List<Integer> results = new ArrayList<>();
            for (int i = 0; i < workSize; i++) {
                results.add(i * 3);

                if (i % 10000 == 0) {
                    Thread.sleep(1);
                }
            }

            System.out.println("[CALLABLE] " + taskId + " completed");
            return results;
        }
    }

    // ==================== DEMONSTRATION CLASSES ====================

    static class ThreadDemonstrator {

        public static void demonstrateInsecureRun() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE: Calling run() directly (CWE-572)");
            System.out.println("=".repeat(60));

            InsecureTask task = new InsecureTask("TASK-INSECURE", 100000);

            System.out.println("\nCalling task.run() directly from main thread...");
            long start = System.currentTimeMillis();

            // INSECURE: Calling run() instead of start()
            task.run();

            long duration = System.currentTimeMillis() - start;

            System.out.println("\nResults:");
            System.out.println("  Task executed on thread: " + Thread.currentThread().getName());
            System.out.println("  Duration: " + duration + "ms");
            System.out.println("  Results count: " + task.getResultCount());
            System.out.println("  Task thread state: " + task.getState());

            System.out.println("\n⚠️ PROBLEM: Task ran on MAIN thread, not a separate thread!");
            System.out.println("  The thread object was created but never started.");
        }

        public static void demonstrateCorrectStart() throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("CORRECT: Calling start() to launch new thread");
            System.out.println("=".repeat(60));

            CorrectTask task = new CorrectTask("TASK-CORRECT", 100000);

            System.out.println("\nCalling task.start() to launch new thread...");
            long start = System.currentTimeMillis();

            // CORRECT: Starting new thread
            task.start();

            // Main thread continues while task runs
            System.out.println("Main thread continues while task runs in background...");

            // Monitor progress
            while (task.isAlive()) {
                System.out.println("  Progress: " + task.getProgress() + "/100000");
                Thread.sleep(10);
            }

            task.join(); // Wait for completion
            long duration = System.currentTimeMillis() - start;

            System.out.println("\nResults:");
            System.out.println("  Task executed on separate thread");
            System.out.println("  Main thread: " + Thread.currentThread().getName());
            System.out.println("  Task thread: " + task.getName() + " (ID: " + task.getId() + ")");
            System.out.println("  Total duration: " + duration + "ms");
            System.out.println("  Task duration: " + task.getDuration() + "ms");
            System.out.println("  Results count: " + task.getResultCount());

            System.out.println("\n✅ CORRECT: Task ran on separate thread, main thread continued!");
        }

        public static void demonstrateRunnableWithThread() throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("RUNNABLE with Thread (Best Practice)");
            System.out.println("=".repeat(60));

            RunnableTask runnable = new RunnableTask("TASK-RUNNABLE", 100000);
            Thread thread = new Thread(runnable, "Runnable-Thread");

            System.out.println("\nStarting thread with Runnable...");
            long start = System.currentTimeMillis();

            thread.start();

            // Wait for completion
            thread.join();

            long duration = System.currentTimeMillis() - start;

            System.out.println("\nResults:");
            System.out.println("  Thread name: " + thread.getName());
            System.out.println("  Duration: " + duration + "ms");
            System.out.println("  Results count: " + runnable.getResultCount());

            System.out.println("\n✅ GOOD: Separates task from threading mechanism");
        }

        public static void demonstrateExecutorService() throws Exception {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("EXECUTOR SERVICE (Modern Approach)");
            System.out.println("=".repeat(60));

            ExecutorService executor = Executors.newFixedThreadPool(3);
            List<Future<List<Integer>>> futures = new ArrayList<>();

            System.out.println("\nSubmitting multiple tasks to executor...");

            // Submit multiple tasks
            for (int i = 0; i < 3; i++) {
                CallableTask task = new CallableTask("TASK-" + i, 50000);
                Future<List<Integer>> future = executor.submit(task);
                futures.add(future);
            }

            // Collect results
            for (int i = 0; i < futures.size(); i++) {
                List<Integer> results = futures.get(i).get();
                System.out.println("  Task " + i + " produced " + results.size() + " results");
            }

            executor.shutdown();
            executor.awaitTermination(1, TimeUnit.MINUTES);

            System.out.println("\n✅ EXCELLENT: Managed thread pool, automatic resource handling");
        }

        public static void demonstrateThreadStates() throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("THREAD STATE DEMONSTRATION");
            System.out.println("=".repeat(60));

            CorrectTask task = new CorrectTask("STATE-DEMO", 100000);

            System.out.println("\nThread states:");
            System.out.println("  After creation: " + task.getState());

            task.start();
            System.out.println("  After start(): " + task.getState());

            Thread.sleep(10);
            System.out.println("  During execution: " + task.getState());

            task.join();
            System.out.println("  After completion: " + task.getState());

            System.out.println("\n⚠️ Note: If we had called run() instead, we'd never see these states!");
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-572: Calling Thread.run() Instead of Thread.start()");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE run() call (CWE-572)");
            System.out.println("  2. Demonstrate CORRECT start() call");
            System.out.println("  3. Demonstrate Runnable with Thread");
            System.out.println("  4. Demonstrate ExecutorService");
            System.out.println("  5. Compare thread states");
            System.out.println("  6. Run parallel tasks comparison");
            System.out.println("  7. Show security analysis");
            System.out.println("  8. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            try {
                switch (choice) {
                    case "1":
                        ThreadDemonstrator.demonstrateInsecureRun();
                        break;

                    case "2":
                        ThreadDemonstrator.demonstrateCorrectStart();
                        break;

                    case "3":
                        ThreadDemonstrator.demonstrateRunnableWithThread();
                        break;

                    case "4":
                        ThreadDemonstrator.demonstrateExecutorService();
                        break;

                    case "5":
                        ThreadDemonstrator.demonstrateThreadStates();
                        break;

                    case "6":
                        compareParallelExecution();
                        break;

                    case "7":
                        showSecurityAnalysis();
                        break;

                    case "8":
                        System.out.println("\nExiting...");
                        scanner.close();
                        return;

                    default:
                        System.out.println("[!] Invalid option");
                }
            } catch (Exception e) {
                System.err.println("Error: " + e.getMessage());
            }
        }
    }

    private static void compareParallelExecution() throws InterruptedException {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("PARALLEL EXECUTION COMPARISON");
        System.out.println("=".repeat(60));

        int taskCount = 3;
        int workSize = 50000;

        System.out.println("\nRunning " + taskCount + " tasks with " + workSize + " iterations each");

        // INSECURE: Sequential execution via run()
        System.out.println("\n🔴 INSECURE: Calling run() sequentially:");
        long startSeq = System.currentTimeMillis();

        for (int i = 0; i < taskCount; i++) {
            InsecureTask task = new InsecureTask("SEQ-" + i, workSize);
            task.run(); // Sequential!
        }

        long seqTime = System.currentTimeMillis() - startSeq;
        System.out.println("  Total time: " + seqTime + "ms");

        // CORRECT: Parallel execution via start()
        System.out.println("\n✅ CORRECT: Using start() for parallel execution:");
        long startPar = System.currentTimeMillis();

        List<CorrectTask> tasks = new ArrayList<>();
        for (int i = 0; i < taskCount; i++) {
            CorrectTask task = new CorrectTask("PAR-" + i, workSize);
            tasks.add(task);
            task.start(); // Parallel!
        }

        // Wait for all tasks
        for (CorrectTask task : tasks) {
            task.join();
        }

        long parTime = System.currentTimeMillis() - startPar;
        System.out.println("  Total time: " + parTime + "ms");

        double speedup = (double) seqTime / parTime;
        System.out.printf("  Speedup: %.2fx\n", speedup);

        System.out.println("\n📊 ANALYSIS:");
        System.out.println("  • run()  : Executes sequentially on main thread");
        System.out.println("  • start(): Executes in parallel on separate threads");
        System.out.println("  • With multiple cores, start() is much faster");
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-572");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-572 VULNERABLE):");
        System.out.println("  1. Calling run() directly:");
        System.out.println("     • Executes on current thread, not new thread");
        System.out.println("     • No parallelism achieved");
        System.out.println("     • Thread object wasted");

        System.out.println("\n  2. Consequences:");
        System.out.println("     • Application runs sequentially");
        System.out.println("     • Poor performance under load");
        System.out.println("     • UI freezes (in GUI apps)");
        System.out.println("     • DoS vulnerability");
        System.out.println("     • Resource underutilization");

        System.out.println("\n  3. Common Mistake Pattern:");
        System.out.println("     Thread t = new Thread(task);");
        System.out.println("     t.run();  // WRONG! Should be t.start()");

        System.out.println("\n✅ CORRECT IMPLEMENTATIONS:");

        System.out.println("\n  1. Thread.start():");
        System.out.println("     • Creates new thread of execution");
        System.out.println("     • run() called on new thread");
        System.out.println("     • True parallelism");

        System.out.println("\n  2. Runnable + Thread:");
        System.out.println("     • Separates task from threading");
        System.out.println("     • More flexible");
        System.out.println("     • Can reuse with thread pools");

        System.out.println("\n  3. ExecutorService (BEST):");
        System.out.println("     • Managed thread pools");
        System.out.println("     • Resource control");
        System.out.println("     • Future for results");
        System.out.println("     • Proper lifecycle management");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. ALWAYS call start(), never run() directly");
        System.out.println("  2. Prefer Runnable over extending Thread");
        System.out.println("  3. Use ExecutorService for production");
        System.out.println("  4. Handle InterruptedException properly");
        System.out.println("  5. Use join() to wait for completion");
        System.out.println("  6. Set meaningful thread names");
        System.out.println("  7. Consider thread priorities");
        System.out.println("  8. Use Callable for return values");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • For simple cases: new Thread(runnable).start()");
        System.out.println("  • For production: ExecutorService");
        System.out.println("  • Never call run() directly!");

        System.out.println("\n📊 COMPARISON:");
        System.out.println("  Method           | New Thread | Parallel | Returns Value");
        System.out.println("  -----------------|------------|----------|--------------");
        System.out.println("  run()            |     ❌     |    ❌    |      ❌");
        System.out.println("  start()          |     ✓      |    ✓     |      ❌");
        System.out.println("  Runnable+Thread  |     ✓      |    ✓     |      ❌");
        System.out.println("  Callable+Future  |     ✓      |    ✓     |      ✓");
        System.out.println("  ExecutorService  |     ✓      |    ✓     |      ✓");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // WRONG - CWE-572");
        System.out.println("  Thread t = new Thread(task);");
        System.out.println("  t.run();  // Runs on current thread!");
        System.out.println("");
        System.out.println("  // CORRECT");
        System.out.println("  Thread t = new Thread(task);");
        System.out.println("  t.start(); // Runs on new thread!");
        System.out.println("");
        System.out.println("  // BETTER");
        System.out.println("  ExecutorService exec = Executors.newCachedThreadPool();");
        System.out.println("  exec.submit(task);");

        System.out.println("\n📈 PERFORMANCE IMPACT (3 tasks, 50k iterations each):");
        System.out.println("  run() sequentially : ~450ms (no parallelism)");
        System.out.println("  start() parallel   : ~150ms (3x faster)");
        System.out.println("  ExecutorService    : ~150ms + better resource mgmt");
    }
}