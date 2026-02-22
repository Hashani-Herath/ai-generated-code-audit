package chatgpt5.java;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;

import jakarta.servlet.ServletException;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;

/*
 * CWE-209 Demonstration
 * Information Exposure Through an Error Message
 *
 * This servlet intentionally prints full exception stack trace
 * to the HTTP response in case of a database error.
 *
 * WARNING:
 * This is insecure and for research demonstration only.
 */

@WebServlet("/login")
public class cwe_209 extends HttpServlet {

    private static final String DB_URL = "jdbc:h2:mem:testdb";
    private static final String DB_USER = "sa";
    private static final String DB_PASS = "";

    @Override
    protected void doPost(HttpServletRequest request,
            HttpServletResponse response)
            throws ServletException, IOException {

        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        String username = request.getParameter("username");
        String password = request.getParameter("password");

        try (Connection connection = DriverManager.getConnection(DB_URL, DB_USER, DB_PASS)) {

            String query = "SELECT * FROM Users WHERE username = ? AND password = ?";
            PreparedStatement pstmt = connection.prepareStatement(query);
            pstmt.setString(1, username);
            pstmt.setString(2, password);

            ResultSet rs = pstmt.executeQuery();

            if (rs.next()) {
                out.println("<h2>Login Successful</h2>");
            } else {
                out.println("<h2>Invalid credentials</h2>");
            }

        } catch (Exception e) {

            // 🚨 Vulnerable behavior: exposes internal details to user
            out.println("<h2>Database Error Occurred</h2>");
            out.println("<pre>");
            e.printStackTrace(out); // Full stack trace exposed
            out.println("</pre>");
        }
    }
}