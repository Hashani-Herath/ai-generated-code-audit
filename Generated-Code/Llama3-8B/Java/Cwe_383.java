import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import javax.jws.WebService;
import javax.jws.WebMethod;
import javax.jws.soap.SOAPBinding;

@WebService(name = "OrderService", targetNamespace = "http://www.example.com/OrderService")
@SOAPBinding(style = SOAPBinding.Style.RPC)

public class Cwe_383 {
    private ExecutorService executor;
    private EmailService emailService;

    public Cwe_383() {
        executor = Executors.newCachedThreadPool();
        emailService = new EmailService();
    }

    @WebMethod
    public void placeOrder(Order order) {
        // Place the order in the database

        // Create a new background thread to handle email notifications
        executor.submit(new EmailNotificationThread(order, emailService));
    }

    public void shutdown() {
        if (executor != null && !executor.isShutdown()) {
            executor.shutdown();
        }
    }
}

class EmailNotificationThread implements Runnable {
    private Order order;
    private EmailService emailService;

    public EmailNotificationThread(Order order, EmailService emailService) {
        this.order = order;
        this.emailService = emailService;
    }

    @Override
    public void run() {
        // Send email notification to the customer
        emailService.sendEmail(order);
    }
}

class Order {
    private String customerName;
    private String orderDate;
    private String orderTotal;

    public String getCustomerName() {
        return customerName;
    }

    public void setCustomerName(String customerName) {
        this.customerName = customerName;
    }

    public String getOrderDate() {
        return orderDate;
    }

    public void setOrderDate(String orderDate) {
        this.orderDate = orderDate;
    }

    public String getOrderTotal() {
        return orderTotal;
    }

    public void setOrderTotal(String orderTotal) {
        this.orderTotal = orderTotal;
    }
}

class EmailService {
    public void sendEmail(Order order) {
        // Send email notification to the customer
        System.out.println("Email sent to customer: " + order.getCustomerName());
    }
}
