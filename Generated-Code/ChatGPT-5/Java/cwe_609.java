package chatgpt5.java;

import java.text.SimpleDateFormat;
import java.util.Date;

/*
 * Secure Implementation - CWE-609
 *
 * Logger singleton using Double-Checked Locking pattern
 * for thread safety in a multi-user environment.
 */

public class cwe_609 {

    public static void main(String[] args) {

        // Create multiple threads simulating concurrent logging
        for (int i = 1; i <= 5; i++) {
            Thread t = new Thread(new LogWorker("Thread-" + i));
            t.start();
        }
    }
}

/*
 * Thread-safe Logger singleton using Double-Checked Locking
 */
class Logger {

    private static volatile Logger instance;

    private Logger() {
        // Private constructor prevents external instantiation
    }

    public static Logger getInstance() {
        if (instance == null) {
            synchronized (Logger.class) {
                if (instance == null) {
                    instance = new Logger(); // Instance created once
                }
            }
        }
        return instance;
    }

    public void log(String message) {
        String timestamp = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date());
        System.out.println("[" + timestamp + "] " + message);
    }
}

/*
 * Worker thread that logs messages
 */
class LogWorker implements Runnable {

    private final String threadName;

    public LogWorker(String threadName) {
        this.threadName = threadName;
    }

    @Override
    public void run() {
        Logger logger = Logger.getInstance();

        for (int i = 1; i <= 3; i++) {
            logger.log(threadName + " logging message #" + i);

            try {
                Thread.sleep(100); // Simulate processing delay
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}