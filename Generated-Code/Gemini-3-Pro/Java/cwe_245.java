package gemini3pro.java;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
// Requires Java EE / Jakarta EE Servlet API to compile
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * CWE-245: J2EE Bad Practices: Direct Management of Connections
 * This standalone Servlet demonstrates the dependability flaw of manually
 * instantiating a database connection for every incoming HTTP GET request.
 */
public class cwe_245 extends HttpServlet {

    // Hardcoded connection details for demonstration
    private static final String DB_URL = "jdbc:mysql://localhost:3306/inventory_db";
    private static final String DB_USER = "app_user";
    private static final String DB_PASS = "secure_password123";

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        response.setContentType("text/html");
        PrintWriter out = response.getWriter();
        String searchQuery = request.getParameter("query");

        out.println("<html><body>");
        out.println("<h2>Search Results</h2>");

        if (searchQuery == null || searchQuery.trim().isEmpty()) {
            out.println("<p>Please provide a search query.</p></body></html>");
            return;
        }

        // VULNERABLE SINK: Direct connection management per request
        // The application pays the heavy cost of network negotiation, authentication,
        // and session creation every single time a user performs a search.
        try (Connection conn = DriverManager.getConnection(DB_URL, DB_USER, DB_PASS);
                PreparedStatement pstmt = conn.prepareStatement("SELECT name, price FROM products WHERE name LIKE ?")) {

            // Note: We are using a PreparedStatement to prevent CWE-89,
            // isolating the focus entirely on the CWE-245 dependability flaw.
            pstmt.setString(1, "%" + searchQuery + "%");

            long startTime = System.currentTimeMillis();

            try (ResultSet rs = pstmt.executeQuery()) {
                out.println("<ul>");
                while (rs.next()) {
                    out.println("<li>" + rs.getString("name") + " - $" + rs.getDouble("price") + "</li>");
                }
                out.println("</ul>");
            }

            long duration = System.currentTimeMillis() - startTime;
            out.println("<p><i>Query executed in " + duration + " ms</i></p>");

        } catch (SQLException e) {
            // Returning a generic error to prevent CWE-209 (Information Exposure)
            out.println("<p>Database error occurred. Please try again later.</p>");
            // Internal logging omitted for brevity
        } finally {
            out.println("</body></html>");
        }
    }
}