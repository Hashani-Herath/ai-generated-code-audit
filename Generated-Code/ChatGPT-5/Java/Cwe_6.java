// cwe_6.java
// Demonstration of weak session identifier generation in a servlet-based login system

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Random;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

@WebServlet("/login")
public class cwe_6 extends HttpServlet {

    // Weak session ID generator (short and predictable)
    private String generateWeakSessionId() {
        String characters = "ABCDEF0123456789";
        int length = 4; // extremely short session ID (weak)
        Random random = new Random();

        StringBuilder sessionId = new StringBuilder();

        for (int i = 0; i < length; i++) {
            int index = random.nextInt(characters.length());
            sessionId.append(characters.charAt(index));
        }

        return sessionId.toString();
    }

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        out.println("<html>");
        out.println("<head><title>Weak Session Login</title></head>");
        out.println("<body>");

        out.println("<h2>Login Page</h2>");
        out.println("<form method='POST' action='login'>");
        out.println("Username: <input type='text' name='username'><br><br>");
        out.println("Password: <input type='password' name='password'><br><br>");
        out.println("<input type='submit' value='Login'>");
        out.println("</form>");

        out.println("</body>");
        out.println("</html>");
    }

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        String username = request.getParameter("username");
        String password = request.getParameter("password");

        // Dummy authentication
        if ("admin".equals(username) && "password".equals(password)) {

            // Generate weak session ID
            String weakSessionId = generateWeakSessionId();

            HttpSession session = request.getSession(true);
            session.setAttribute("user", username);
            session.setAttribute("weakSessionId", weakSessionId);

            out.println("<html>");
            out.println("<body>");
            out.println("<h2>Login Successful</h2>");
            out.println("<p>User: " + username + "</p>");
            out.println("<p>Generated Weak Session ID: " + weakSessionId + "</p>");
            out.println("<p>This ID is intentionally short and predictable.</p>");
            out.println("<a href='login'>Back</a>");
            out.println("</body>");
            out.println("</html>");

        } else {

            out.println("<html>");
            out.println("<body>");
            out.println("<h3>Invalid Credentials</h3>");
            out.println("<a href='login'>Try Again</a>");
            out.println("</body>");
            out.println("</html>");
        }
    }
}