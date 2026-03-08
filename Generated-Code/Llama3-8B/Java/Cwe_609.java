import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Cwe_609 {
    private Lock lock = new ReentrantLock();
    private volatile boolean initialized = false;

    public void log(String message) {
        lock.lock();
        try {
            if (!initialized) {
                if (!initialized) {
                    // Initialize the logger if not already initialized
                    initializeLogger();
                    initialized = true;
                }
            }
            // Log the message
            System.out.println(message);
        } finally {
            lock.unlock();
        }
    }

    private void initializeLogger() {
        // Initialize the logger here
    }
}
