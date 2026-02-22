package deepseek.java;

import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;
import java.time.*;
import java.time.format.*;
import javax.mail.*;
import javax.mail.internet.*;
import java.io.*;
import java.util.logging.*;

// Note: This example includes email concepts but doesn't require actual mail server
// In production, you would need javax.mail dependency

public class cwe_383 {

    // ==================== ORDER CLASS ====================

    static class Order {
        private final String orderId;
        private final String customerId;
        private final String customerEmail;
        private final String customerName;
        private final List<OrderItem> items;
        private final double totalAmount;
        private final LocalDateTime orderTime;
        private OrderStatus status;
        private String shippingAddress;
        private String paymentMethod;

        public enum OrderStatus {
            PENDING, PROCESSING, SHIPPED, DELIVERED, CANCELLED
        }

        public Order(String orderId, String customerId, String customerEmail, String customerName) {
            this.orderId = orderId;
            this.customerId = customerId;
            this.customerEmail = customerEmail;
            this.customerName = customerName;
            this.items = new ArrayList<>();
            this.totalAmount = 0.0;
            this.orderTime = LocalDateTime.now();
            this.status = OrderStatus.PENDING;
        }

        public String getOrderId() {
            return orderId;
        }

        public String getCustomerEmail() {
            return customerEmail;
        }

        public String getCustomerName() {
            return customerName;
        }

        public List<OrderItem> getItems() {
            return items;
        }

        public double getTotalAmount() {
            return items.stream().mapToDouble(OrderItem::getSubtotal).sum();
        }

        public OrderStatus getStatus() {
            return status;
        }

        public void setStatus(OrderStatus status) {
            this.status = status;
        }

        public void addItem(OrderItem item) {
            items.add(item);
        }

        public String getEmailContent() {
            StringBuilder content = new StringBuilder();
            content.append("Dear ").append(customerName).append(",\n\n");
            content.append("Thank you for your order #").append(orderId).append(".\n\n");
            content.append("Order Details:\n");
            for (OrderItem item : items) {
                content.append("  - ").append(item.getProductName())
                        .append(" x ").append(item.getQuantity())
                        .append(" @ $").append(item.getPrice())
                        .append(" = $").append(item.getSubtotal()).append("\n");
            }
            content.append("\nTotal: $").append(String.format("%.2f", getTotalAmount()));
            content.append("\n\nWe will notify you when your order ships.");
            content.append("\n\nThank you for shopping with us!\n");
            return content.toString();
        }
    }

    static class OrderItem {
        private final String productId;
        private final String productName;
        private final int quantity;
        private final double price;

        public OrderItem(String productId, String productName, int quantity, double price) {
            this.productId = productId;
            this.productName = productName;
            this.quantity = quantity;
            this.price = price;
        }

        public String getProductName() {
            return productName;
        }

        public int getQuantity() {
            return quantity;
        }

        public double getPrice() {
            return price;
        }

        public double getSubtotal() {
            return price * quantity;
        }
    }

    // ==================== INSECURE ORDER SERVICE (CWE-383 VULNERABLE)
    // ====================

    static class InsecureOrderService {

        private static final Logger LOGGER = Logger.getLogger(InsecureOrderService.class.getName());
        private final Map<String, Order> orders = new ConcurrentHashMap<>();
        private final AtomicInteger orderCounter = new AtomicInteger(0);

        // INSECURE: Creating raw threads manually
        public Order processOrder(String customerId, String customerEmail, String customerName) {
            System.out.println("\n[INSECURE] Processing order for: " + customerName);

            String orderId = "ORD-" + orderCounter.incrementAndGet();
            Order order = new Order(orderId, customerId, customerEmail, customerName);

            // Add sample items
            order.addItem(new OrderItem("P001", "Laptop", 1, 999.99));
            order.addItem(new OrderItem("P002", "Mouse", 2, 29.99));

            orders.put(orderId, order);

            // INSECURE: Creating new thread for each order
            EmailNotificationThread notificationThread = new EmailNotificationThread(order, "order_confirmation");

            // INSECURE: No thread pooling, no resource management
            notificationThread.start();

            System.out.println("[INSECURE] Order processed: " + orderId);
            return order;
        }

        // INSECURE: Thread class defined inside service
        private class EmailNotificationThread extends Thread {
            private final Order order;
            private final String notificationType;

            public EmailNotificationThread(Order order, String notificationType) {
                this.order = order;
                this.notificationType = notificationType;
            }

            @Override
            public void run() {
                try {
                    // Simulate email sending
                    System.out.println("[INSECURE-THREAD] Sending " + notificationType +
                            " email for order: " + order.getOrderId());

                    // Simulate network delay
                    Thread.sleep(2000);

                    // Generate email content
                    String emailContent = order.getEmailContent();

                    // Simulate email sending
                    System.out.println("[INSECURE-THREAD] Email sent to: " +
                            order.getCustomerEmail());
                    System.out.println("[INSECURE-THREAD] Content:\n" + emailContent);

                } catch (InterruptedException e) {
                    LOGGER.severe("Email thread interrupted: " + e.getMessage());
                } catch (Exception e) {
                    LOGGER.severe("Failed to send email: " + e.getMessage());
                }
            }
        }

        // INSECURE: Another thread for different notification
        public void sendShipmentNotification(String orderId) {
            Order order = orders.get(orderId);
            if (order != null) {
                Thread shipmentThread = new Thread(() -> {
                    try {
                        System.out.println("[INSECURE-THREAD] Sending shipment notification");
                        Thread.sleep(1500);
                        System.out.println("[INSECURE-THREAD] Shipment notification sent");
                    } catch (InterruptedException e) {
                        LOGGER.severe("Shipment thread interrupted");
                    }
                });
                shipmentThread.start();
            }
        }

        public Order getOrder(String orderId) {
            return orders.get(orderId);
        }

        public int getActiveThreadCount() {
            // INSECURE: Can't easily track threads
            return Thread.activeCount();
        }
    }

    // ==================== INSECURE WITH THREAD POOL (STILL PROBLEMATIC)
    // ====================

    static class InsecureThreadPoolService {

        private final Map<String, Order> orders = new ConcurrentHashMap<>();
        private final AtomicInteger orderCounter = new AtomicInteger(0);

        // INSECURE: Unbounded cached thread pool - can create too many threads
        private final ExecutorService emailExecutor = Executors.newCachedThreadPool();

        // INSECURE: No proper shutdown handling
        private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(5);

        public Order processOrder(String customerId, String customerEmail, String customerName) {
            String orderId = "ORD-" + orderCounter.incrementAndGet();
            Order order = new Order(orderId, customerId, customerEmail, customerName);

            orders.put(orderId, order);

            // Still problematic - no resource limits
            emailExecutor.submit(() -> sendEmail(order, "order_confirmation"));

            return order;
        }

        private void sendEmail(Order order, String type) {
            try {
                Thread.sleep(2000);
                System.out.println("[INSECURE-POOL] Email sent: " + type);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }

        // INSECURE: No cleanup - thread pool never shut down
    }

    // ==================== SECURE ORDER SERVICE (CWE-383 MITIGATED)
    // ====================

    static class SecureOrderService {

        private static final Logger LOGGER = Logger.getLogger(SecureOrderService.class.getName());
        private static final int CORE_POOL_SIZE = 5;
        private static final int MAX_POOL_SIZE = 20;
        private static final int QUEUE_CAPACITY = 100;
        private static final long KEEP_ALIVE_TIME = 60;

        private final Map<String, Order> orders = new ConcurrentHashMap<>();
        private final AtomicInteger orderCounter = new AtomicInteger(0);

        // SECURE: Bounded thread pool with queue
        private final ExecutorService emailExecutor;

        // SECURE: Single thread for monitoring
        private final ScheduledExecutorService monitorExecutor;

        // SECURE: Queue for tracking pending notifications
        private final BlockingQueue<NotificationTask> notificationQueue;

        // SECURE: Metrics for monitoring
        private final AtomicInteger emailsSent = new AtomicInteger(0);
        private final AtomicInteger emailsFailed = new AtomicInteger(0);
        private final AtomicLong totalEmailTime = new AtomicLong(0);

        public SecureOrderService() {
            // SECURE: Bounded thread pool with work queue
            this.emailExecutor = new ThreadPoolExecutor(
                    CORE_POOL_SIZE,
                    MAX_POOL_SIZE,
                    KEEP_ALIVE_TIME,
                    TimeUnit.SECONDS,
                    new ArrayBlockingQueue<>(QUEUE_CAPACITY),
                    new ThreadFactory() {
                        private final AtomicInteger threadCounter = new AtomicInteger(0);

                        @Override
                        public Thread newThread(Runnable r) {
                            Thread t = new Thread(r);
                            t.setName("EmailWorker-" + threadCounter.incrementAndGet());
                            t.setDaemon(true);
                            t.setUncaughtExceptionHandler((thread, e) -> {
                                LOGGER.severe("Uncaught exception in " + thread.getName() + ": " + e);
                            });
                            return t;
                        }
                    },
                    new RejectedExecutionHandler() {
                        @Override
                        public void rejectedExecution(Runnable r, ThreadPoolExecutor executor) {
                            // SECURE: Handle rejected tasks gracefully
                            if (r instanceof EmailTask) {
                                EmailTask task = (EmailTask) r;
                                LOGGER.warning("Email task rejected for order: " + task.orderId);
                                notificationQueue.offer(new NotificationTask(
                                        task.orderId, task.notificationType, true));
                            }
                        }
                    });

            // SECURE: Monitor thread for queue status
            this.monitorExecutor = Executors.newScheduledThreadPool(1, r -> {
                Thread t = new Thread(r);
                t.setName("EmailMonitor");
                t.setDaemon(true);
                return t;
            });

            this.notificationQueue = new LinkedBlockingQueue<>();

            // Start monitoring
            startMonitoring();
        }

        // SECURE: Process order with controlled threading
        public Order processOrder(String customerId, String customerEmail, String customerName) {
            System.out.println("\n[SECURE] Processing order for: " + customerName);

            String orderId = "ORD-" + orderCounter.incrementAndGet();
            Order order = new Order(orderId, customerId, customerEmail, customerName);

            // Add sample items
            order.addItem(new OrderItem("P001", "Laptop", 1, 999.99));
            order.addItem(new OrderItem("P002", "Mouse", 2, 29.99));

            orders.put(orderId, order);

            // SECURE: Submit task to bounded thread pool
            EmailTask task = new EmailTask(order, "order_confirmation");

            try {
                emailExecutor.submit(task);
                System.out.println("[SECURE] Email task submitted for order: " + orderId);
            } catch (RejectedExecutionException e) {
                // SECURE: Handle rejection gracefully
                LOGGER.warning("Email task rejected, queueing for later: " + orderId);
                notificationQueue.offer(new NotificationTask(orderId, "order_confirmation", false));
            }

            System.out.println("[SECURE] Order processed: " + orderId);
            return order;
        }

        // SECURE: Separate task class
        private class EmailTask implements Runnable {
            private final Order order;
            private final String notificationType;
            private final String orderId;

            public EmailTask(Order order, String notificationType) {
                this.order = order;
                this.notificationType = notificationType;
                this.orderId = order.getOrderId();
            }

            @Override
            public void run() {
                long startTime = System.currentTimeMillis();
                Thread currentThread = Thread.currentThread();

                try {
                    System.out.println("[SECURE-TASK] " + currentThread.getName() +
                            " sending email for order: " + orderId);

                    // SECURE: Set thread context for logging
                    MDC.put("orderId", orderId);
                    MDC.put("customerEmail", order.getCustomerEmail());

                    // Simulate email preparation
                    String emailContent = prepareEmail(order, notificationType);

                    // Simulate sending with retry logic
                    boolean sent = sendWithRetry(order.getCustomerEmail(), emailContent, 3);

                    if (sent) {
                        emailsSent.incrementAndGet();
                        long duration = System.currentTimeMillis() - startTime;
                        totalEmailTime.addAndGet(duration);

                        System.out.println("[SECURE-TASK] Email sent successfully in " +
                                duration + "ms");

                        // Update order status
                        order.setStatus(Order.OrderStatus.PROCESSING);

                    } else {
                        emailsFailed.incrementAndGet();
                        LOGGER.warning("Failed to send email for order: " + orderId);

                        // Queue for retry
                        notificationQueue.offer(new NotificationTask(
                                orderId, notificationType, true));
                    }

                } catch (Exception e) {
                    emailsFailed.incrementAndGet();
                    LOGGER.log(Level.SEVERE, "Error sending email for order: " + orderId, e);

                    // Queue for retry
                    notificationQueue.offer(new NotificationTask(
                            orderId, notificationType, true));

                } finally {
                    MDC.clear();
                }
            }

            private String prepareEmail(Order order, String type) {
                // Simulate email template processing
                return order.getEmailContent();
            }

            private boolean sendWithRetry(String to, String content, int maxRetries) {
                for (int i = 0; i < maxRetries; i++) {
                    try {
                        // Simulate email sending
                        Thread.sleep(1000);

                        // Simulate random failure (20% chance)
                        if (Math.random() < 0.2) {
                            throw new IOException("Simulated network error");
                        }

                        return true;

                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                        return false;
                    } catch (IOException e) {
                        LOGGER.warning("Retry " + (i + 1) + " failed: " + e.getMessage());
                        try {
                            Thread.sleep(1000 * (i + 1)); // Exponential backoff
                        } catch (InterruptedException ie) {
                            Thread.currentThread().interrupt();
                            return false;
                        }
                    }
                }
                return false;
            }
        }

        // SECURE: Notification task for queue
        private static class NotificationTask {
            final String orderId;
            final String type;
            final boolean highPriority;

            NotificationTask(String orderId, String type, boolean highPriority) {
                this.orderId = orderId;
                this.type = type;
                this.highPriority = highPriority;
            }
        }

        // SECURE: Start monitoring thread
        private void startMonitoring() {
            monitorExecutor.scheduleAtFixedRate(() -> {
                try {
                    // Monitor thread pool status
                    if (emailExecutor instanceof ThreadPoolExecutor) {
                        ThreadPoolExecutor tpe = (ThreadPoolExecutor) emailExecutor;

                        System.out.println("\n[SECURE-MONITOR] Pool Status:");
                        System.out.println("  Active threads: " + tpe.getActiveCount());
                        System.out.println("  Pool size: " + tpe.getPoolSize());
                        System.out.println("  Queue size: " + tpe.getQueue().size());
                        System.out.println("  Completed tasks: " + tpe.getCompletedTaskCount());
                        System.out.println("  Emails sent: " + emailsSent.get());
                        System.out.println("  Emails failed: " + emailsFailed.get());

                        if (tpe.getQueue().size() > QUEUE_CAPACITY * 0.8) {
                            LOGGER.warning("Email queue near capacity!");
                        }
                    }

                    // Process queued notifications
                    processNotificationQueue();

                } catch (Exception e) {
                    LOGGER.log(Level.SEVERE, "Error in monitor thread", e);
                }
            }, 30, 30, TimeUnit.SECONDS);
        }

        // SECURE: Process queued notifications
        private void processNotificationQueue() {
            List<NotificationTask> tasks = new ArrayList<>();
            notificationQueue.drainTo(tasks);

            for (NotificationTask task : tasks) {
                Order order = orders.get(task.orderId);
                if (order != null) {
                    try {
                        if (task.highPriority) {
                            // High priority tasks get submitted immediately
                            emailExecutor.submit(new EmailTask(order, task.type));
                        } else {
                            // Low priority tasks go back to queue if full
                            if (emailExecutor instanceof ThreadPoolExecutor) {
                                ThreadPoolExecutor tpe = (ThreadPoolExecutor) emailExecutor;
                                if (tpe.getQueue().remainingCapacity() > 10) {
                                    emailExecutor.submit(new EmailTask(order, task.type));
                                } else {
                                    notificationQueue.offer(task);
                                }
                            }
                        }
                    } catch (RejectedExecutionException e) {
                        notificationQueue.offer(task);
                    }
                }
            }
        }

        // SECURE: Proper shutdown
        public void shutdown() {
            System.out.println("\n[SECURE] Shutting down order service...");

            // Stop accepting new tasks
            emailExecutor.shutdown();
            monitorExecutor.shutdown();

            try {
                // Wait for existing tasks to complete
                if (!emailExecutor.awaitTermination(30, TimeUnit.SECONDS)) {
                    emailExecutor.shutdownNow();

                    if (!emailExecutor.awaitTermination(30, TimeUnit.SECONDS)) {
                        LOGGER.severe("Email executor did not terminate");
                    }
                }

                if (!monitorExecutor.awaitTermination(5, TimeUnit.SECONDS)) {
                    monitorExecutor.shutdownNow();
                }

            } catch (InterruptedException e) {
                emailExecutor.shutdownNow();
                monitorExecutor.shutdownNow();
                Thread.currentThread().interrupt();
            }

            // Final statistics
            System.out.println("[SECURE] Final statistics:");
            System.out.println("  Emails sent successfully: " + emailsSent.get());
            System.out.println("  Emails failed: " + emailsFailed.get());
            System.out.println("  Average email time: " +
                    (emailsSent.get() > 0 ? totalEmailTime.get() / emailsSent.get() : 0) + "ms");
        }

        public Order getOrder(String orderId) {
            return orders.get(orderId);
        }

        public int getQueueSize() {
            return notificationQueue.size();
        }
    }

    // ==================== MOCK MDC FOR LOGGING ====================

    static class MDC {
        private static final ThreadLocal<Map<String, String>> context = ThreadLocal.withInitial(HashMap::new);

        public static void put(String key, String value) {
            context.get().put(key, value);
        }

        public static void clear() {
            context.remove();
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class OrderServiceDemonstrator {

        public static void demonstrateInsecureService() throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE ORDER SERVICE (CWE-383)");
            System.out.println("=".repeat(60));

            InsecureOrderService service = new InsecureOrderService();

            // Process multiple orders
            System.out.println("\n🔴 Processing 5 orders...");
            for (int i = 1; i <= 5; i++) {
                Order order = service.processOrder(
                        "CUST" + i,
                        "customer" + i + "@example.com",
                        "Customer " + i);
                System.out.println("  Order created: " + order.getOrderId());
            }

            // Show thread count
            Thread.sleep(1000);
            System.out.println("\n[INSECURE] Active threads: " + service.getActiveThreadCount());

            // Problems with this approach
            System.out.println("\n⚠️ PROBLEMS:");
            System.out.println("  • Threads not managed - unlimited creation");
            System.out.println("  • No thread reuse");
            System.out.println("  • Can't track or limit threads");
            System.out.println("  • No error handling for failed emails");
            System.out.println("  • No retry mechanism");
            System.out.println("  • JVM may run out of threads under load");
        }

        public static void demonstrateSecureService() throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE ORDER SERVICE (MITIGATED)");
            System.out.println("=".repeat(60));

            SecureOrderService service = new SecureOrderService();

            try {
                // Process multiple orders
                System.out.println("\n✅ Processing 10 orders...");
                for (int i = 1; i <= 10; i++) {
                    Order order = service.processOrder(
                            "CUST" + i,
                            "customer" + i + "@example.com",
                            "Customer " + i);
                    System.out.println("  Order created: " + order.getOrderId());
                    Thread.sleep(100);
                }

                // Let tasks complete
                System.out.println("\nWaiting for email tasks to complete...");
                Thread.sleep(10000);

            } finally {
                service.shutdown();
            }

            System.out.println("\n✅ Secure service maintains control over threading.");
        }

        public static void demonstrateLoadTest() throws InterruptedException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("LOAD TEST COMPARISON");
            System.out.println("=".repeat(60));

            int orderCount = 50;

            // Test insecure
            System.out.println("\n🔴 Testing insecure service with " + orderCount + " orders...");
            InsecureOrderService insecure = new InsecureOrderService();
            long start = System.currentTimeMillis();

            for (int i = 1; i <= orderCount; i++) {
                insecure.processOrder("CUST" + i, "cust" + i + "@test.com", "Customer " + i);
            }

            long insecureTime = System.currentTimeMillis() - start;
            System.out.println("  Insecure processing time: " + insecureTime + "ms");

            // Let threads spin up
            Thread.sleep(2000);
            System.out.println("  Threads created: " + insecure.getActiveThreadCount());

            // Test secure
            System.out.println("\n✅ Testing secure service with " + orderCount + " orders...");
            SecureOrderService secure = new SecureOrderService();
            start = System.currentTimeMillis();

            try {
                for (int i = 1; i <= orderCount; i++) {
                    secure.processOrder("CUST" + i, "cust" + i + "@test.com", "Customer " + i);
                    if (i % 10 == 0) {
                        Thread.sleep(100); // Allow queue to process
                    }
                }

                long secureTime = System.currentTimeMillis() - start;
                System.out.println("  Secure processing time: " + secureTime + "ms");
                System.out.println("  Queue size: " + secure.getQueueSize());

                Thread.sleep(5000);

            } finally {
                secure.shutdown();
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-383: J2EE Bad Practices - Direct Use of Threads");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE threading (CWE-383)");
            System.out.println("  2. Demonstrate SECURE threading (Mitigated)");
            System.out.println("  3. Run load test comparison");
            System.out.println("  4. Show security analysis");
            System.out.println("  5. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            try {
                switch (choice) {
                    case "1":
                        OrderServiceDemonstrator.demonstrateInsecureService();
                        break;

                    case "2":
                        OrderServiceDemonstrator.demonstrateSecureService();
                        break;

                    case "3":
                        OrderServiceDemonstrator.demonstrateLoadTest();
                        break;

                    case "4":
                        showSecurityAnalysis();
                        break;

                    case "5":
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

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-383");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-383 VULNERABLE):");
        System.out.println("  1. Direct Thread Creation:");
        System.out.println("     • new Thread().start() for each request");
        System.out.println("     • Unbounded thread creation");
        System.out.println("     • No thread reuse");

        System.out.println("\n  2. Resource Exhaustion:");
        System.out.println("     • Each thread consumes memory (≈1MB stack)");
        System.out.println("     • OS thread limit reached quickly");
        System.out.println("     • Application becomes unresponsive");

        System.out.println("\n  3. Lack of Control:");
        System.out.println("     • Can't limit concurrent threads");
        System.out.println("     • No queue for pending tasks");
        System.out.println("     • No monitoring or metrics");

        System.out.println("\n  4. Error Handling:");
        System.out.println("     • Uncaught exceptions kill threads");
        System.out.println("     • No retry mechanism");
        System.out.println("     • Lost notifications");

        System.out.println("\n  5. J2EE Violations:");
        System.out.println("     • Container can't manage threads");
        System.out.println("     • Transaction boundaries broken");
        System.out.println("     • Security context not propagated");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Managed Thread Pools:");
        System.out.println("     • Bounded thread pool with queue");
        System.out.println("     • Thread reuse");
        System.out.println("     • Configurable limits");

        System.out.println("\n  2. Resource Control:");
        System.out.println("     • Core/max pool sizes");
        System.out.println("     • Work queue capacity");
        System.out.println("     • Keep-alive timing");

        System.out.println("\n  3. Monitoring & Metrics:");
        System.out.println("     • Track active threads");
        System.out.println("     • Monitor queue size");
        System.out.println("     • Success/failure counts");

        System.out.println("\n  4. Error Handling:");
        System.out.println("     • Retry with backoff");
        System.out.println("     • Rejection handling");
        System.out.println("     • Dead letter queue");

        System.out.println("\n  5. Clean Shutdown:");
        System.out.println("     • Graceful termination");
        System.out.println("     • Task completion waiting");
        System.out.println("     • Resource cleanup");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Never create threads directly in J2EE");
        System.out.println("  2. Use ManagedExecutorService in Java EE");
        System.out.println("  3. Configure thread pools with bounds");
        System.out.println("  4. Implement proper rejection policies");
        System.out.println("  5. Add monitoring and alerts");
        System.out.println("  6. Handle thread interruption properly");
        System.out.println("  7. Use work queues for overload");
        System.out.println("  8. Implement retry with backoff");
        System.out.println("  9. Set thread names for debugging");
        System.out.println("  10. Always shutdown executors gracefully");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use ExecutorService with bounded pools");
        System.out.println("  • Configure based on expected load");
        System.out.println("  • Implement circuit breakers for failures");
        System.out.println("  • Monitor thread pool metrics");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  new Thread(() -> {");
        System.out.println("      sendEmail(order);");
        System.out.println("  }).start();");
        System.out.println("");
        System.out.println("  // SECURE");
        System.out.println("  executor.submit(() -> {");
        System.out.println("      try {");
        System.out.println("          sendEmail(order);");
        System.out.println("      } catch (Exception e) {");
        System.out.println("          log.error(\"Failed\", e);");
        System.out.println("          retryQueue.offer(order);");
        System.out.println("      }");
        System.out.println("  });");

        System.out.println("\n📊 THREAD POOL COMPARISON:");
        System.out.println("  Strategy        | Scalable | Controllable | Production-Ready");
        System.out.println("  ----------------|----------|--------------|-----------------");
        System.out.println("  Raw Threads     |    ❌    |      ❌      |        ❌");
        System.out.println("  Cached Pool     |    ✓     |      ⚠️      |        ⚠️");
        System.out.println("  Fixed Pool      |    ✓     |      ✓       |        ✓");
        System.out.println("  Work Stealing   |    ✓     |      ✓       |        ✓");

        System.out.println("\n⚠️ REMEMBER: In J2EE containers, use container-managed threads!");
    }
}