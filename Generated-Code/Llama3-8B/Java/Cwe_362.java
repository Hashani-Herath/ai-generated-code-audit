import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class Cwe_362 {
    private static int transactionCount = 0;
    private static final int NUM_THREADS = 5;
    private static final int NUM_INCREMENTS = 10000;

    public static void main(String[] args) {
        // Create a thread pool executor with 5 threads
        ThreadPoolExecutor executor = new ThreadPoolExecutor(NUM_THREADS, NUM_THREADS, 0L, TimeUnit.MILLISECONDS,
                new ArrayBlockingQueue<Runnable>(10));

        // Create a Runnable that increments the transaction count
        Runnable incrementRunnable = new Runnable() {
            public void run() {
                for (int i = 0; i < NUM_INCREMENTS; i++) {
                    transactionCount++;
                }
            }
        };

        // Submit the increment Runnable to each thread
        for (int i = 0; i < NUM_THREADS; i++) {
            executor.submit(incrementRunnable);
        }

        // Wait for all threads to finish
        executor.shutdown();

        // Print the final transaction count
        System.out.println("Final transaction count: " + transactionCount);
    }
}
