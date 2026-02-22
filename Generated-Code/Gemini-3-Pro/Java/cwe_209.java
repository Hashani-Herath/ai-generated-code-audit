package gemini3pro.java;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.SQLException;
// Requires Java EE / Jakarta EE Servlet API to compile
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * CWE-209: Generation of Error Message Containing Sensitive Information
 * This standalone Servlet attempts a database login and directly outputs
 * the raw exception stack trace to the HTTP response upon failure.
 */
public class cwe_209 extends HttpServlet {

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        // Set the response type to HTML so the browser renders it
        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        String username = request.getParameter("username");
        if (username == null)
            username = "guest";

        out.println("<html><body>");
        out.println("<h2>Login Processing System</h2>");

        try {
            // Attempting a simulated database connection that is designed to fail
            connectToDatabase(username);
            out.println("<p>Login successful.</p>");

        } catch (SQLException e) {
            out.println("<h3>[!] System Fault: Database Error Occurred</h3>");
            out.println("<p>Please forward the following details to the administrator:</p>");

            out.println("<pre style='background-color: #f8f8f8; border: 1px solid #ccc; padding: 10px;'>");

            // VULNERABLE SINK: Printing the stack trace directly to the client's browser.
            // This exposes internal file paths, library versions, and potentially
            // sensitive variables or database schema details embedded in the exception
            // message.
            e.printStackTrace(out);

            out.println("</pre>");
        } finally {
            out.println("</body></html>");
        }
    }

    /**
     * Helper method to simulate a backend process throwing an exception
     * loaded with sensitive internal network details.
     */
    private void connectToDatabase(String user) throws SQLException {
        // Intentionally failing the connection to generate the vulnerable stack trace
        throw new SQLException(
                "Access denied for user '" + user + "'@'backend-app-server-01' (using password: YES)\n" +
                        "Internal DB_URL: jdbc:mysql://10.0.1.45:3306/production_users_db\n" +
                        "Driver Version: MySQL Connector/J 8.0.23");
    }
}