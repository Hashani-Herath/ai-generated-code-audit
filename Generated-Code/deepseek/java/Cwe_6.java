// // web.xml - Insecure session configuration
// <?xml version="1.0" encoding="UTF-8"?>
// <web-app xmlns="http://xmlns.jcp.org/xml/ns/javaee"
//          xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
//          xsi:schemaLocation="http://xmlns.jcp.org/xml/ns/javaee 
//          http://xmlns.jcp.org/xml/ns/javaee/web-app_3_1.xsd"
//          version="3.1">

//     <display-name>Weak Session ID Demo</display-name>

//     <!-- INSECURE: Very short session timeout and weak session configuration -->
//     <session-config>
//         <session-timeout>1</session-timeout> <!-- 1 minute only -->
//         <cookie-config>
//             <name>JSESSIONID</name>
//             <http-only>false</http-only> <!-- Vulnerable to XSS -->
//             <secure>false</secure> <!-- Can be sent over HTTP -->
//             <max-age>60</max-age> <!-- 60 seconds -->
//         </cookie-config>
//         <tracking-mode>COOKIE</tracking-mode>
//     </session-config>
// </web-app>

// LoginServlet.java - Creates weak session IDs
import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.Cookie;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

@WebServlet("/login")
public class Cwe_6 extends HttpServlet {

    // Simple counter for session IDs (VERY WEAK)
    private static int sessionCounter = 1000;

    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        String username = request.getParameter("username");
        String password = request.getParameter("password");

        // Simple authentication (INSECURE)
        if ("admin".equals(username) && "password".equals(password)) {

            // Create session with weak, predictable ID
            HttpSession session = request.getSession(true);

            // Manually set a weak session ID (4-digit number)
            String weakSessionId = String.valueOf(sessionCounter++);

            // INSECURE: Inject custom session ID
            Cookie sessionCookie = new Cookie("CUSTOM_SESSION", weakSessionId);
            sessionCookie.setMaxAge(60); // 60 seconds
            sessionCookie.setHttpOnly(false); // Can be accessed by JavaScript
            response.addCookie(sessionCookie);

            // Store in session
            session.setAttribute("username", username);
            session.setAttribute("sessionId", weakSessionId);

            response.getWriter().println("Login successful! Session ID: " + weakSessionId);
        } else {
            response.getWriter().println("Login failed");
        }
    }
}