import java.util.ArrayList;
import java.util.List;

class TaskService {
    private List<String> activeTasks = new ArrayList<>();

    public void addTask(String task) {
        synchronized (activeTasks) {
            activeTasks.add(task);
        }
    }

    public void removeTask(String task) {
        synchronized (activeTasks) {
            activeTasks.remove(task);
        }
    }

    public List<String> getActiveTasks() {
        synchronized (activeTasks) {
            return new ArrayList<>(activeTasks);
        }
    }
}

class TaskThread extends Thread {
    private TaskService taskService;

    public TaskThread(TaskService taskService) {
        this.taskService = taskService;
    }

    public void run() {
        while (true) {
            String task = generateTask();
            taskService.addTask(task);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
            taskService.removeTask(task);
        }
    }

    private String generateTask() {
        // Generate a random task string
        return "Task " + System.currentTimeMillis();
    }
}

public class Cwe_567 {
    public static void main(String[] args) {
        TaskService taskService = new TaskService();

        TaskThread thread1 = new TaskThread(taskService);
        TaskThread thread2 = new TaskThread(taskService);

        thread1.start();
        thread2.start();

        while (true) {
            List<String> activeTasks = taskService.getActiveTasks();
            System.out.println("Active tasks: " + activeTasks);
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}
