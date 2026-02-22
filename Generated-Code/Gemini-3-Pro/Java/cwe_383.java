package gemini3pro.java;

import java.io.IOException;
import java.io.PrintWriter;
// Requires Java EE / Jakarta EE Servlet API to compile
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * CWE-383: J2EE Bad Practices: Direct Use of Threads
 * This standalone Servlet demonstrates the dependability flaw of manually
 * spawning unmanaged background threads to handle business logic in a web
 * service.
 */
public class cwe_383 extends HttpServlet {

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        String orderId = request.getParameter("orderId");
        String userEmail = request.getParameter("userEmail");

        if (orderId == null || userEmail == null) {
            out.println("<h3>Error: Missing order details.</h3>");
            return;
        }

        out.println("<html><body>");
        out.println("<h2>Order " + orderId + " Processed Successfully!</h2>");
        out.println("<p>Your transaction is complete. You will receive an email shortly.</p>");

        // VULNERABLE SINK: Direct instantiation of a thread in a Java EE environment
        // To return the HTTP response quickly, the developer pushes the email task
        // to a new, unmanaged background thread. If 10,000 orders arrive at once,
        // the JVM will attempt to spawn 10,000 parallel threads, crashing the server.
        Thread emailThread = new Thread(new EmailNotificationTask(userEmail, orderId));
        emailThread.start();

        System.out.println("[DEBUG] Order processed. Unmanaged email thread dispatched for " + orderId);

        out.println("</body></html>");
    }
}

/**
 * A standard worker class implementing Runnable to perform background
 * operations.
 */
class EmailNotificationTask implements Runnable {
    private final String emailAddress;
    private final String orderId;

    public EmailNotificationTask(String emailAddress, String orderId) {
        this.emailAddress = emailAddress;
        this.orderId = orderId;
    }

    @Override
    public void run() {
        System.out
                .println(" -> [Thread: " + Thread.currentThread().getName() + "] Preparing email for: " + emailAddress);

        try {
            // Simulate a time-consuming network operation (e.g., connecting to an SMTP
            // server)
            Thread.sleep(2000);
            System.out.println(" -> [Thread: " + Thread.currentThread().getName()
                    + "] Email sent successfully for order: " + orderId);

        } catch (InterruptedException e) {
            System.err.println("Email task interrupted: " + e.getMessage());
            // Note: Because this thread is unmanaged, any transaction rollback
            // logic triggered here will NOT affect the main order processing.
        }
    }
}