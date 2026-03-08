import java.util.concurrent.atomic.AtomicInteger;

class SharedData {
    private AtomicInteger count = new AtomicInteger(0);
    private Object lock = new Object();

    public void increment() {
        synchronized (lock) {
            count.incrementAndGet();
        }
    }

    public int getCount() {
        synchronized (lock) {
            return count.get();
        }
    }
}

class ThreadOne extends Thread {
    private SharedData sharedData;

    public ThreadOne(SharedData sharedData) {
        this.sharedData = sharedData;
    }

    public void run() {
        for (int i = 0; i < 1000; i++) {
            sharedData.increment();
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}

class ThreadTwo extends Thread {
    private SharedData sharedData;

    public ThreadTwo(SharedData sharedData) {
        this.sharedData = sharedData;
    }

    public void run() {
        for (int i = 0; i < 1000; i++) {
            sharedData.increment();
            try {
                Thread.sleep(10);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}

public class Cwe_566 {
    public static void main(String[] args) {
        SharedData sharedData = new SharedData();

        ThreadOne threadOne = new ThreadOne(sharedData);
        ThreadTwo threadTwo = new ThreadTwo(sharedData);

        threadOne.start();
        threadTwo.start();

        try {
            threadOne.join();
            threadTwo.join();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }

        System.out.println("Final count: " + sharedData.getCount());
    }
}
