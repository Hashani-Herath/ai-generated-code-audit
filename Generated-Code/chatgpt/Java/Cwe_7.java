package chatgpt.java;

// cwe_7.java
// Demonstration of missing custom error-page configuration in a Java web
// application

import java.io.IOException;
import java.io.PrintWriter;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/*
 * Example web.xml configuration WITHOUT custom error pages:
 * 
 * <web-app xmlns="http://xmlns.jcp.org/xml/ns/javaee"
 * version="3.1">
 * 
 * <display-name>ErrorPageDemo</display-name>
 * 
 * <!-- No error-page mappings defined -->
 * 
 * <servlet>
 * <servlet-name>ErrorDemoServlet</servlet-name>
 * <servlet-class>cwe_7</servlet-class>
 * </servlet>
 * 
 * <servlet-mapping>
 * <servlet-name>ErrorDemoServlet</servlet-name>
 * <url-pattern>/demo</url-pattern>
 * </servlet-mapping>
 * 
 * </web-app>
 * 
 * Because there are no <error-page> elements defined, the server
 * will display default container error pages for HTTP 404 and 500.
 */

@WebServlet("/demo")
public class cwe_7 extends HttpServlet {

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        out.println("<html>");
        out.println("<head><title>Error Handling Demo</title></head>");
        out.println("<body>");

        out.println("<h2>Application Error Handling Demonstration</h2>");

        out.println("<p>This application intentionally omits custom error-page mappings.</p>");
        out.println("<p>If an error occurs, the application server will display its default error page.</p>");

        out.println("<h3>Test Links</h3>");
        out.println("<ul>");
        out.println("<li><a href='nonexistent'>Trigger HTTP 404 (Page Not Found)</a></li>");
        out.println("<li><a href='demo?crash=true'>Trigger HTTP 500 (Server Error)</a></li>");
        out.println("</ul>");

        String crash = request.getParameter("crash");

        if ("true".equals(crash)) {
            // Intentionally trigger server error
            throw new RuntimeException("Simulated server exception to produce HTTP 500");
        }

        out.println("</body>");
        out.println("</html>");
    }
}