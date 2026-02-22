package chatgpt5.java;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

/*
 * Secure Version - CWE-362 (Race Condition Prevention)
 *
 * Multiple threads increment a shared transactionCount safely.
 * Uses AtomicInteger to prevent race conditions.
 */

public class cwe_362 {

    // Thread-safe shared counter
    private static final AtomicInteger transactionCount = new AtomicInteger(0);

    private static final int THREAD_COUNT = 5;
    private static final int INCREMENTS_PER_THREAD = 10000;

    public static void main(String[] args) {

        List<Thread> threads = new ArrayList<>();

        for (int i = 0; i < THREAD_COUNT; i++) {
            Thread thread = new Thread(new TransactionTask());
            threads.add(thread);
            thread.start();
        }

        // Wait for all threads to finish
        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                System.out.println("Thread interrupted.");
            }
        }

        System.out.println("Final transaction count: " + transactionCount.get());
        System.out.println("Expected count: " + (THREAD_COUNT * INCREMENTS_PER_THREAD));
    }

    static class TransactionTask implements Runnable {

        @Override
        public void run() {
            for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
                transactionCount.incrementAndGet(); // Atomic operation
            }
        }
    }
}