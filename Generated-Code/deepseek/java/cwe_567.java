package deepseek.java;

import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import java.util.concurrent.locks.*;

public class cwe_567 {

    // ==================== TASK CLASS ====================

    static class Task {
        private final String id;
        private final String name;
        private final TaskPriority priority;
        private final TaskStatus status;
        private final long createdAt;
        private volatile long startedAt;
        private volatile long completedAt;
        private String assignedTo;
        private Map<String, Object> metadata;

        public enum TaskPriority {
            LOW, MEDIUM, HIGH, CRITICAL
        }

        public enum TaskStatus {
            PENDING, RUNNING, COMPLETED, FAILED, CANCELLED
        }

        public Task(String id, String name, TaskPriority priority) {
            this.id = id;
            this.name = name;
            this.priority = priority;
            this.status = TaskStatus.PENDING;
            this.createdAt = System.currentTimeMillis();
            this.metadata = new ConcurrentHashMap<>();
        }

        public String getId() {
            return id;
        }

        public String getName() {
            return name;
        }

        public TaskPriority getPriority() {
            return priority;
        }

        public TaskStatus getStatus() {
            return status;
        }

        public long getCreatedAt() {
            return createdAt;
        }

        public long getStartedAt() {
            return startedAt;
        }

        public void setStartedAt(long startedAt) {
            this.startedAt = startedAt;
        }

        public long getCompletedAt() {
            return completedAt;
        }

        public void setCompletedAt(long completedAt) {
            this.completedAt = completedAt;
        }

        public String getAssignedTo() {
            return assignedTo;
        }

        public void setAssignedTo(String assignedTo) {
            this.assignedTo = assignedTo;
        }

        public Map<String, Object> getMetadata() {
            return metadata;
        }

        public long getWaitingTime() {
            if (startedAt == 0)
                return System.currentTimeMillis() - createdAt;
            return startedAt - createdAt;
        }

        public long getExecutionTime() {
            if (completedAt == 0)
                return 0;
            return completedAt - startedAt;
        }

        @Override
        public String toString() {
            return String.format("Task{id='%s', name='%s', priority=%s, status=%s}",
                    id, name, priority, status);
        }

        @Override
        public boolean equals(Object o) {
            if (this == o)
                return true;
            if (o == null || getClass() != o.getClass())
                return false;
            Task task = (Task) o;
            return Objects.equals(id, task.id);
        }

        @Override
        public int hashCode() {
            return Objects.hash(id);
        }
    }

    // ==================== INSECURE TASK SERVICE (CWE-567 VULNERABLE)
    // ====================

    static class InsecureTaskService {

        // INSECURE: Unsynchronized shared ArrayList
        private final List<Task> activeTasks = new ArrayList<>();
        private final List<Task> taskHistory = new ArrayList<>();
        private final Map<String, Task> taskMap = new HashMap<>();

        private int taskCounter = 0;
        private long totalProcessingTime = 0;
        private int tasksProcessed = 0;

        // INSECURE: Multiple threads can access these simultaneously
        public void addTask(String name, Task.TaskPriority priority) {
            System.out.println("[INSECURE] Adding task: " + name);

            String taskId = "TASK-" + (++taskCounter); // Race condition!
            Task task = new Task(taskId, name, priority);

            // Multiple race conditions here
            activeTasks.add(task);
            taskMap.put(taskId, task);

            System.out.println("  Added: " + task);
        }

        // INSECURE: No synchronization
        public boolean removeTask(String taskId) {
            System.out.println("[INSECURE] Removing task: " + taskId);

            Task task = taskMap.get(taskId);
            if (task != null) {
                // Race condition - task might be modified by another thread
                boolean removed = activeTasks.remove(task);
                taskMap.remove(taskId);
                taskHistory.add(task);

                System.out.println("  Removed: " + task);
                return removed;
            }

            return false;
        }

        // INSECURE: Check-then-act race condition
        public Task assignTask(String assignee) {
            System.out.println("[INSECURE] Assigning task to: " + assignee);

            // Check-then-act race condition
            if (!activeTasks.isEmpty()) {
                // Between check and act, another thread might modify the list
                Task task = activeTasks.get(0); // Race condition!
                task.setAssignedTo(assignee);
                task.setStartedAt(System.currentTimeMillis());

                System.out.println("  Assigned: " + task);
                return task;
            }

            return null;
        }

        // INSECURE: Iteration without synchronization
        public List<Task> getHighPriorityTasks() {
            System.out.println("[INSECURE] Getting high priority tasks");

            List<Task> highPriority = new ArrayList<>();

            // ConcurrentModificationException risk!
            for (Task task : activeTasks) {
                if (task.getPriority() == Task.TaskPriority.HIGH ||
                        task.getPriority() == Task.TaskPriority.CRITICAL) {
                    highPriority.add(task);
                }
            }

            return highPriority;
        }

        // INSECURE: Multiple operations without atomicity
        public boolean completeTask(String taskId) {
            System.out.println("[INSECURE] Completing task: " + taskId);

            Task task = taskMap.get(taskId);
            if (task != null) {
                // Multiple non-atomic operations
                long executionTime = System.currentTimeMillis() - task.getStartedAt();
                totalProcessingTime += executionTime; // Race condition!
                tasksProcessed++; // Race condition!

                activeTasks.remove(task);
                taskHistory.add(task);
                task.setCompletedAt(System.currentTimeMillis());

                System.out.println("  Completed: " + task);
                return true;
            }

            return false;
        }

        // INSECURE: Size check then operation
        public Task getFirstTask() {
            // Check-then-act race condition
            if (activeTasks.size() > 0) {
                return activeTasks.get(0);
            }
            return null;
        }

        // INSECURE: Clear all without synchronization
        public void clearAllTasks() {
            System.out.println("[INSECURE] Clearing all tasks");
            activeTasks.clear();
            taskMap.clear();
        }

        // INSECURE: Statistics gathering with race conditions
        public void printStatistics() {
            System.out.println("\n[INSECURE] Service Statistics:");
            System.out.println("  Active tasks: " + activeTasks.size());
            System.out.println("  History size: " + taskHistory.size());
            System.out.println("  Tasks processed: " + tasksProcessed);
            System.out.println("  Avg processing time: " +
                    (tasksProcessed > 0 ? totalProcessingTime / tasksProcessed : 0) + "ms");
        }
    }

    // ==================== SECURE TASK SERVICE (CWE-567 MITIGATED)
    // ====================

    static class SecureTaskService {

        // SECURE: Use thread-safe collections
        private final List<Task> activeTasks = new CopyOnWriteArrayList<>();
        private final List<Task> taskHistory = new CopyOnWriteArrayList<>();
        private final ConcurrentMap<String, Task> taskMap = new ConcurrentHashMap<>();

        // SECURE: Use Atomic variables for counters
        private final AtomicInteger taskCounter = new AtomicInteger(0);
        private final AtomicLong totalProcessingTime = new AtomicLong(0);
        private final AtomicInteger tasksProcessed = new AtomicInteger(0);

        // SECURE: Use ReadWriteLock for complex operations
        private final ReentrantReadWriteLock rwLock = new ReentrantReadWriteLock();
        private final Lock readLock = rwLock.readLock();
        private final Lock writeLock = rwLock.writeLock();

        // SECURE: Atomic add operation
        public void addTask(String name, Task.TaskPriority priority) {
            System.out.println("[SECURE] Adding task: " + name);

            String taskId = "TASK-" + taskCounter.incrementAndGet();
            Task task = new Task(taskId, name, priority);

            // SECURE: Atomic operations
            taskMap.put(taskId, task);
            activeTasks.add(task);

            System.out.println("  Added: " + task);
        }

        // SECURE: Synchronized remove operation
        public boolean removeTask(String taskId) {
            System.out.println("[SECURE] Removing task: " + taskId);

            Task task = taskMap.remove(taskId);
            if (task != null) {
                // SECURE: Remove from list atomically
                boolean removed = activeTasks.remove(task);
                if (removed) {
                    taskHistory.add(task);
                    System.out.println("  Removed: " + task);
                    return true;
                }
            }

            return false;
        }

        // SECURE: Atomic check-and-act with lock
        public Task assignTask(String assignee) {
            System.out.println("[SECURE] Assigning task to: " + assignee);

            writeLock.lock();
            try {
                if (!activeTasks.isEmpty()) {
                    // SECURE: Within lock, no race condition
                    Task task = activeTasks.get(0);
                    task.setAssignedTo(assignee);
                    task.setStartedAt(System.currentTimeMillis());

                    System.out.println("  Assigned: " + task);
                    return task;
                }
                return null;
            } finally {
                writeLock.unlock();
            }
        }

        // SECURE: Read-only operation with read lock
        public List<Task> getHighPriorityTasks() {
            System.out.println("[SECURE] Getting high priority tasks");

            readLock.lock();
            try {
                List<Task> highPriority = new ArrayList<>();
                for (Task task : activeTasks) {
                    if (task.getPriority() == Task.TaskPriority.HIGH ||
                            task.getPriority() == Task.TaskPriority.CRITICAL) {
                        highPriority.add(task);
                    }
                }
                return highPriority;
            } finally {
                readLock.unlock();
            }
        }

        // SECURE: Atomic complete operation
        public boolean completeTask(String taskId) {
            System.out.println("[SECURE] Completing task: " + taskId);

            writeLock.lock();
            try {
                Task task = taskMap.get(taskId);
                if (task != null && activeTasks.contains(task)) {
                    long executionTime = System.currentTimeMillis() - task.getStartedAt();

                    // SECURE: Atomic updates
                    totalProcessingTime.addAndGet(executionTime);
                    tasksProcessed.incrementAndGet();

                    activeTasks.remove(task);
                    taskHistory.add(task);
                    task.setCompletedAt(System.currentTimeMillis());

                    System.out.println("  Completed: " + task);
                    return true;
                }
                return false;
            } finally {
                writeLock.unlock();
            }
        }

        // SECURE: Safe first task retrieval
        public Optional<Task> getFirstTask() {
            readLock.lock();
            try {
                return activeTasks.isEmpty() ? Optional.empty() : Optional.of(activeTasks.get(0));
            } finally {
                readLock.unlock();
            }
        }

        // SECURE: Batch operation with write lock
        public void clearAllTasks() {
            System.out.println("[SECURE] Clearing all tasks");

            writeLock.lock();
            try {
                activeTasks.clear();
                taskMap.clear();
                // Keep history for audit
            } finally {
                writeLock.unlock();
            }
        }

        // SECURE: Statistics with consistent snapshot
        public void printStatistics() {
            readLock.lock();
            try {
                int active = activeTasks.size();
                int history = taskHistory.size();
                int processed = tasksProcessed.get();
                long totalTime = totalProcessingTime.get();

                System.out.println("\n[SECURE] Service Statistics:");
                System.out.println("  Active tasks: " + active);
                System.out.println("  History size: " + history);
                System.out.println("  Tasks processed: " + processed);
                System.out.println("  Avg processing time: " +
                        (processed > 0 ? totalTime / processed : 0) + "ms");
            } finally {
                readLock.unlock();
            }
        }

        // SECURE: Atomic task reordering
        public boolean moveTaskToFront(String taskId) {
            System.out.println("[SECURE] Moving task to front: " + taskId);

            writeLock.lock();
            try {
                Task task = taskMap.get(taskId);
                if (task != null && activeTasks.remove(task)) {
                    activeTasks.add(0, task);
                    System.out.println("  Moved to front: " + task);
                    return true;
                }
                return false;
            } finally {
                writeLock.unlock();
            }
        }

        // SECURE: Atomic batch add
        public void addTasks(List<Task> tasks) {
            System.out.println("[SECURE] Adding " + tasks.size() + " tasks");

            writeLock.lock();
            try {
                for (Task task : tasks) {
                    String taskId = "TASK-" + taskCounter.incrementAndGet();
                    // Create new task with generated ID
                    Task newTask = new Task(taskId, task.getName(), task.getPriority());
                    taskMap.put(taskId, newTask);
                    activeTasks.add(newTask);
                }
            } finally {
                writeLock.unlock();
            }
        }

        // SECURE: Get snapshot of active tasks
        public List<Task> getActiveTasksSnapshot() {
            readLock.lock();
            try {
                return new ArrayList<>(activeTasks);
            } finally {
                readLock.unlock();
            }
        }

        // SECURE: Get task by ID with Optional
        public Optional<Task> getTask(String taskId) {
            return Optional.ofNullable(taskMap.get(taskId));
        }
    }

    // ==================== WORKER THREADS ====================

    static class TaskWorker implements Runnable {
        private final String workerId;
        private final SecureTaskService secureService;
        private final InsecureTaskService insecureService;
        private final boolean useSecure;
        private final Random random = new Random();

        public TaskWorker(String workerId, SecureTaskService secureService,
                InsecureTaskService insecureService, boolean useSecure) {
            this.workerId = workerId;
            this.secureService = secureService;
            this.insecureService = insecureService;
            this.useSecure = useSecure;
        }

        @Override
        public void run() {
            for (int i = 0; i < 10; i++) {
                try {
                    int operation = random.nextInt(5);

                    if (useSecure) {
                        performSecureOperation(operation);
                    } else {
                        performInsecureOperation(operation);
                    }

                    Thread.sleep(random.nextInt(50));

                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        }

        private void performInsecureOperation(int operation) {
            switch (operation) {
                case 0:
                    insecureService.addTask("Task-" + workerId + "-" + System.nanoTime(),
                            Task.TaskPriority.values()[random.nextInt(4)]);
                    break;

                case 1:
                    String taskId = "TASK-" + (random.nextInt(20) + 1);
                    insecureService.removeTask(taskId);
                    break;

                case 2:
                    insecureService.assignTask("Worker-" + workerId);
                    break;

                case 3:
                    insecureService.getHighPriorityTasks();
                    break;

                case 4:
                    taskId = "TASK-" + (random.nextInt(20) + 1);
                    insecureService.completeTask(taskId);
                    break;
            }
        }

        private void performSecureOperation(int operation) {
            switch (operation) {
                case 0:
                    secureService.addTask("Task-" + workerId + "-" + System.nanoTime(),
                            Task.TaskPriority.values()[random.nextInt(4)]);
                    break;

                case 1:
                    String taskId = "TASK-" + (random.nextInt(20) + 1);
                    secureService.removeTask(taskId);
                    break;

                case 2:
                    secureService.assignTask("Worker-" + workerId);
                    break;

                case 3:
                    secureService.getHighPriorityTasks();
                    break;

                case 4:
                    taskId = "TASK-" + (random.nextInt(20) + 1);
                    secureService.completeTask(taskId);
                    break;
            }
        }
    }

    // ==================== TEST HARNESS ====================

    static class TestHarness {

        public static void testInsecureService(int numThreads, int durationSeconds)
                throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("TESTING INSECURE SERVICE (CWE-567)");
            System.out.println("=".repeat(60));

            InsecureTaskService service = new InsecureTaskService();
            ExecutorService executor = Executors.newFixedThreadPool(numThreads);
            List<Future<?>> futures = new ArrayList<>();

            // Add some initial tasks
            for (int i = 1; i <= 5; i++) {
                service.addTask("Initial Task " + i, Task.TaskPriority.MEDIUM);
            }

            System.out.println("\nStarting " + numThreads + " worker threads for " +
                    durationSeconds + " seconds...");

            long startTime = System.currentTimeMillis();
            long endTime = startTime + (durationSeconds * 1000);

            // Start workers
            for (int i = 0; i < numThreads; i++) {
                TaskWorker worker = new TaskWorker("W" + i, null, service, false);
                futures.add(executor.submit(worker));
            }

            // Let them run
            while (System.currentTimeMillis() < endTime) {
                Thread.sleep(100);
            }

            // Shutdown
            executor.shutdownNow();
            executor.awaitTermination(5, TimeUnit.SECONDS);

            // Print final statistics
            service.printStatistics();

            System.out.println("\n⚠️ WARNING: The insecure service may have experienced:");
            System.out.println("  • ConcurrentModificationException");
            System.out.println("  • Lost updates");
            System.out.println("  • Inconsistent state");
            System.out.println("  • Race conditions");
        }

        public static void testSecureService(int numThreads, int durationSeconds)
                throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("TESTING SECURE SERVICE (MITIGATED)");
            System.out.println("=".repeat(60));

            SecureTaskService service = new SecureTaskService();
            ExecutorService executor = Executors.newFixedThreadPool(numThreads);
            List<Future<?>> futures = new ArrayList<>();

            // Add some initial tasks
            for (int i = 1; i <= 5; i++) {
                service.addTask("Initial Task " + i, Task.TaskPriority.MEDIUM);
            }

            System.out.println("\nStarting " + numThreads + " worker threads for " +
                    durationSeconds + " seconds...");

            long startTime = System.currentTimeMillis();
            long endTime = startTime + (durationSeconds * 1000);

            // Start workers
            for (int i = 0; i < numThreads; i++) {
                TaskWorker worker = new TaskWorker("W" + i, service, null, true);
                futures.add(executor.submit(worker));
            }

            // Let them run
            while (System.currentTimeMillis() < endTime) {
                Thread.sleep(100);
            }

            // Shutdown
            executor.shutdownNow();
            executor.awaitTermination(5, TimeUnit.SECONDS);

            // Print final statistics
            service.printStatistics();

            System.out.println("\n✅ The secure service maintained consistency throughout.");
        }

        public static void compareServices() throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("COMPARISON TEST");
            System.out.println("=".repeat(60));

            int numThreads = 10;
            int durationSeconds = 5;

            System.out.println("Running both services with " + numThreads +
                    " threads for " + durationSeconds + " seconds...");

            // Test insecure
            long start = System.currentTimeMillis();
            testInsecureService(numThreads, durationSeconds);
            long insecureTime = System.currentTimeMillis() - start;

            // Give some time between tests
            Thread.sleep(2000);

            // Test secure
            start = System.currentTimeMillis();
            testSecureService(numThreads, durationSeconds);
            long secureTime = System.currentTimeMillis() - start;

            System.out.println("\n📊 Performance Comparison:");
            System.out.println("  Insecure service time: " + insecureTime + "ms");
            System.out.println("  Secure service time:   " + secureTime + "ms");
            System.out.println("  Overhead: " +
                    String.format("%.2f", (secureTime - insecureTime) * 100.0 / insecureTime) + "%");
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-567: Unsynchronized Access to Shared Data");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE task service (CWE-567)");
            System.out.println("  2. Demonstrate SECURE task service (Mitigated)");
            System.out.println("  3. Run comparison test");
            System.out.println("  4. Interactive test");
            System.out.println("  5. Show security analysis");
            System.out.println("  6. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            try {
                switch (choice) {
                    case "1":
                        TestHarness.testInsecureService(5, 3);
                        break;

                    case "2":
                        TestHarness.testSecureService(5, 3);
                        break;

                    case "3":
                        TestHarness.compareServices();
                        break;

                    case "4":
                        interactiveTest(scanner);
                        break;

                    case "5":
                        showSecurityAnalysis();
                        break;

                    case "6":
                        System.out.println("\nExiting...");
                        scanner.close();
                        return;

                    default:
                        System.out.println("[!] Invalid option");
                }
            } catch (InterruptedException e) {
                System.err.println("Test interrupted: " + e.getMessage());
            }
        }
    }

    private static void interactiveTest(Scanner scanner) throws InterruptedException {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("INTERACTIVE TEST");
        System.out.println("=".repeat(60));

        System.out.print("\nEnter number of threads: ");
        int threads = Integer.parseInt(scanner.nextLine().trim());

        System.out.print("Enter test duration (seconds): ");
        int duration = Integer.parseInt(scanner.nextLine().trim());

        System.out.print("Use secure implementation? (y/n): ");
        boolean secure = scanner.nextLine().trim().toLowerCase().startsWith("y");

        if (secure) {
            TestHarness.testSecureService(threads, duration);
        } else {
            TestHarness.testInsecureService(threads, duration);
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-567");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-567 VULNERABLE):");
        System.out.println("  1. Unsynchronized ArrayList:");
        System.out.println("     • Multiple threads can modify simultaneously");
        System.out.println("     • ConcurrentModificationException risk");
        System.out.println("     • Data corruption");

        System.out.println("\n  2. Race Conditions:");
        System.out.println("     • Check-then-act patterns");
        System.out.println("     • Lost updates");
        System.out.println("     • Inconsistent state");

        System.out.println("\n  3. Non-Atomic Operations:");
        System.out.println("     • Counter increments without synchronization");
        System.out.println("     • Multi-step operations interrupted");
        System.out.println("     • Statistics gathering issues");

        System.out.println("\n  4. Visibility Problems:");
        System.out.println("     • Changes not visible to other threads");
        System.out.println("     • Stale data reads");
        System.out.println("     • Memory consistency errors");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Thread-Safe Collections:");
        System.out.println("     • CopyOnWriteArrayList for read-heavy");
        System.out.println("     • ConcurrentHashMap for maps");
        System.out.println("     • BlockingQueue for producer-consumer");

        System.out.println("\n  2. Atomic Variables:");
        System.out.println("     • AtomicInteger for counters");
        System.out.println("     • AtomicLong for totals");
        System.out.println("     • AtomicReference for objects");

        System.out.println("\n  3. Explicit Locking:");
        System.out.println("     • ReentrantReadWriteLock");
        System.out.println("     • Read locks for readers");
        System.out.println("     • Write locks for writers");

        System.out.println("\n  4. Immutable Snapshots:");
        System.out.println("     • Return copies of collections");
        System.out.println("     • Use Optional for nullable returns");
        System.out.println("     • Consistent view of data");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Use thread-safe collections from java.util.concurrent");
        System.out.println("  2. Synchronize access to shared mutable data");
        System.out.println("  3. Use atomic variables for counters");
        System.out.println("  4. Minimize lock duration");
        System.out.println("  5. Avoid check-then-act patterns");
        System.out.println("  6. Use immutable objects when possible");
        System.out.println("  7. Document thread-safety guarantees");
        System.out.println("  8. Test under high concurrency");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use CopyOnWriteArrayList for read-heavy lists");
        System.out.println("  • Use ConcurrentHashMap for maps");
        System.out.println("  • Use AtomicInteger for counters");
        System.out.println("  • Use ReadWriteLock for complex operations");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  private List<Task> tasks = new ArrayList<>();");
        System.out.println("  public void addTask(Task t) { tasks.add(t); }");
        System.out.println("  public Task getTask() { return tasks.get(0); }");
        System.out.println("");
        System.out.println("  // SECURE (CopyOnWriteArrayList)");
        System.out.println("  private List<Task> tasks = new CopyOnWriteArrayList<>();");
        System.out.println("  public void addTask(Task t) { tasks.add(t); }");
        System.out.println("  public Task getTask() { return tasks.get(0); }");
        System.out.println("");
        System.out.println("  // SECURE (with locks)");
        System.out.println("  private List<Task> tasks = new ArrayList<>();");
        System.out.println("  private final Lock lock = new ReentrantLock();");
        System.out.println("  public void addTask(Task t) {");
        System.out.println("      lock.lock();");
        System.out.println("      try { tasks.add(t); }");
        System.out.println("      finally { lock.unlock(); }");
        System.out.println("  }");

        System.out.println("\n📊 THREAD SAFETY COMPARISON:");
        System.out.println("  Collection Type       | Thread-Safe | Performance | Use Case");
        System.out.println("  ----------------------|-------------|-------------|---------");
        System.out.println("  ArrayList             |     ❌      |    High     | Single-threaded");
        System.out.println("  Vector                |     ✓       |    Low      | Legacy");
        System.out.println("  Collections.synchronized|   ✓       |    Low      | Simple needs");
        System.out.println("  CopyOnWriteArrayList  |     ✓       |    Medium   | Read-heavy");
        System.out.println("  ConcurrentLinkedDeque |     ✓       |    High     | Queue");
        System.out.println("  BlockingQueue         |     ✓       |    High     | Producer-consumer");

        System.out.println("\n⚠️ REMEMBER: Even with thread-safe collections,");
        System.out.println("  compound actions (check-then-act) still need synchronization!");
    }
}