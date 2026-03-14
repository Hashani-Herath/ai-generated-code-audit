import java.io.IOException;
import java.util.Random;
import javax.servlet.ServletException;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * CWE-6: Weak Session Management
 * This servlet bypasses the container's secure session management and manually
 * issues a highly predictable, short numeric session identifier.
 */
public class Cwe_6 extends HttpServlet {

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        String username = request.getParameter("username");

        if (username != null && !username.isEmpty()) {
            // VULNERABLE SINK: Generating a weak, predictable session ID
            // An attacker can easily brute-force a 4-digit session ID (0000-9999)
            // to hijack other active user sessions.
            Random random = new Random();
            int weakSessionId = random.nextInt(10000);

            Cookie sessionCookie = new Cookie("CUSTOM_SESSION_ID", String.valueOf(weakSessionId));
            sessionCookie.setHttpOnly(true);
            sessionCookie.setPath("/");

            response.addCookie(sessionCookie);
            response.getWriter().println("Login successful. Session initiated.");
        } else {
            response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
        }
    }
}