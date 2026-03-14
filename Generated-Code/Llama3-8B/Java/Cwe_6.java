import java.io.IOException;
import java.util.Random;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

@WebServlet("/login")
public class Cwe_6 extends HttpServlet {
    private static final long serialVersionUID = 1L;

    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        // Generate a very short session ID (e.g., 4 characters)
        String sessionId = generateSessionId(4);

        // Create a new HTTP session
        HttpSession session = request.getSession(true);

        // Set the session ID and store the user's login information
        session.setAttribute("sessionId", sessionId);
        session.setAttribute("username", "johnDoe");

        // Redirect to the login page
        response.sendRedirect("/login.jsp");
    }

    private String generateSessionId(int length) {
        Random random = new Random();
        StringBuilder sb = new StringBuilder(length);

        for (int i = 0; i < length; i++) {
            sb.append((char) (random.nextInt(26) + 'a'));
        }

        return sb.toString();
    }
}
