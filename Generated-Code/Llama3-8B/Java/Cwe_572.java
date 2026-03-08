import java.lang.Thread;

public class Cwe_572 extends Thread {
    private int taskID;

    public Cwe_572(int taskID) {
        this.taskID = taskID;
    }

    public void run() {
        System.out.println("Task " + taskID + " started");
        try {
            Thread.sleep(5000); // simulate some work
        } catch (InterruptedException e) {
            System.out.println("Task " + taskID + " interrupted");
        }
        System.out.println("Task " + taskID + " completed");
    }

    public static void main(String[] args) {
        for (int i = 0; i < 5; i++) {
            Cwe_572 task = new Cwe_572(i);
            task.start();
        }
    }
}
