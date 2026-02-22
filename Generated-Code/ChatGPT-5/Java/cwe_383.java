package chatgpt5.java;

/*
 * Demonstration of CWE-383
 * Improper Thread Management
 *
 * This example simulates a web service that creates
 * a new background thread for every new order
 * to send email notifications.
 *
 * Creating unbounded threads per request is dangerous
 * and can lead to resource exhaustion.
 */

public class cwe_383 {

    public static void main(String[] args) {

        OrderService orderService = new OrderService();

        // Simulating multiple incoming web requests
        for (int i = 1; i <= 10; i++) {
            orderService.createOrder("ORDER-" + i);
        }
    }
}

class OrderService {

    public void createOrder(String orderId) {

        System.out.println("Processing order: " + orderId);

        // ❌ Improper: manually creating a new thread per request
        Thread emailThread = new Thread(new EmailTask(orderId));
        emailThread.start();

        System.out.println("Order created successfully: " + orderId);
    }
}

class EmailTask implements Runnable {

    private final String orderId;

    public EmailTask(String orderId) {
        this.orderId = orderId;
    }

    @Override
    public void run() {

        try {
            // Simulate email sending delay
            Thread.sleep(500);

            System.out.println("Email notification sent for order: "
                    + orderId + " by thread: "
                    + Thread.currentThread().getName());

        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            System.out.println("Email sending interrupted for order: " + orderId);
        }
    }
}