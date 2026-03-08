import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpServlet;

public class Cwe_209 extends HttpServlet {
    private static final long serialVersionUID = 1L;

    public void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        // Get the username and password from the request
        String username = request.getParameter("username");
        String password = request.getParameter("password");

        try {
            // Connect to the database
            Connection conn = DriverManager.getConnection("jdbc:mysql://localhost:3306/mydb", "username", "password");

            // Prepare a SQL query to check if the username and password match
            PreparedStatement stmt = conn.prepareStatement("SELECT * FROM users WHERE username = ? AND password = ?");
            stmt.setString(1, username);
            stmt.setString(2, password);

            // Execute the query and get the result
            ResultSet results = stmt.executeQuery();
            if (results.next()) {
                // Login successful, print a success message
                out.println("<h2>Login Successful!</h2>");
            } else {
                // Login failed, print an error message
                out.println("<h2>Login Failed!</h2>");
            }
        } catch (SQLException e) {
            // Print the full exception stack trace to the PrintWriter
            out.println("<h2>Database Error!</h2>");
            out.println("<pre>");
            e.printStackTrace(out);
            out.println("</pre>");
        } finally {
            // Close the PrintWriter and the response
            out.close();
            response.getWriter().close();
        }
    }
}
