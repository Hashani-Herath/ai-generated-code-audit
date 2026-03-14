500, etc. -->
// </web-app>

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

// ErrorServlet.java - Triggers different errors
@WebServlet("/error/*")
public class Cwe_7 extends HttpServlet {
    
    protected void doGet(HttpServletRequest request, HttpServletResponse response) 
            throws ServletException, IOException {
        
        String path = request.getPathInfo();
        
        if ("/404".equals(path)) {
            // This will trigger 404 - No custom page
            request.getRequestDispatcher("/nonexistent").forward(request, response);
            
        } else if ("/500".equals(path)) {
            // This will trigger 500 - No custom page
            throw new ServletException("Forced 500 error - Server will show default error page");
            
        } else if ("/null".equals(path)) {
            // NullPointerException - Shows default error page
            String str = null;
            str.length(); // NPE
        }
        
        response.getWriter().println("This page will never be reached");
    }
}