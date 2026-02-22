package deepseek.java;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.servlet.annotation.*;
import java.io.*;
import java.sql.*;
import java.util.*;
import java.util.logging.*;
import java.nio.file.*;
import java.time.*;
import java.time.format.*;

@WebServlet("/login")
public class cwe_209 extends HttpServlet {

    private static final long serialVersionUID = 1L;

    // Logger for secure error logging
    private static final Logger LOGGER = Logger.getLogger(cwe_209.class.getName());

    // Database configuration
    private static final String DB_URL = "jdbc:h2:mem:testdb;DB_CLOSE_DELAY=-1";
    private static final String DB_USER = "sa";
    private static final String DB_PASSWORD = "";

    // Custom error messages (no sensitive details)
    private static final String ERROR_DB_CONNECTION = "System temporarily unavailable. Please try again later.";
    private static final String ERROR_INVALID_CREDENTIALS = "Invalid username or password.";
    private static final String ERROR_USER_NOT_FOUND = "Invalid username or password.";
    private static final String ERROR_ACCOUNT_LOCKED = "Account temporarily locked. Please contact support.";
    private static final String ERROR_GENERAL = "An error occurred. Please try again.";

    // Track failed attempts (simple in-memory - use Redis in production)
    private static final Map<String, FailedAttempt> failedAttempts = new HashMap<>();

    @Override
    public void init() throws ServletException {
        super.init();
        try {
            initializeDatabase();
            LOGGER.info("Database initialized successfully");
        } catch (Exception e) {
            LOGGER.severe("Failed to initialize database: " + e.getMessage());
            throw new ServletException("Database initialization failed", e);
        }
    }

    private void initializeDatabase() throws SQLException {
        try (Connection conn = DriverManager.getConnection(DB_URL, DB_USER, DB_PASSWORD);
                Statement stmt = conn.createStatement()) {

            // Create users table
            String createTableSQL = """
                    CREATE TABLE IF NOT EXISTS users (
                        id INT AUTO_INCREMENT PRIMARY KEY,
                        username VARCHAR(50) UNIQUE NOT NULL,
                        password_hash VARCHAR(255) NOT NULL,
                        full_name VARCHAR(100),
                        email VARCHAR(100),
                        role VARCHAR(20),
                        failed_attempts INT DEFAULT 0,
                        locked_until TIMESTAMP NULL,
                        last_login TIMESTAMP NULL,
                        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                    )
                    """;
            stmt.execute(createTableSQL);

            // Insert sample users (with hashed passwords - using simple hash for demo)
            String insertSQL = """
                    INSERT INTO users (username, password_hash, full_name, email, role)
                    SELECT * FROM (
                        SELECT 'alice', '5f4dcc3b5aa765d61d8327deb882cf99', 'Alice Johnson', 'alice@example.com', 'ADMIN' UNION
                        SELECT 'bob', '5f4dcc3b5aa765d61d8327deb882cf99', 'Bob Smith', 'bob@example.com', 'USER' UNION
                        SELECT 'charlie', '5f4dcc3b5aa765d61d8327deb882cf99', 'Charlie Brown', 'charlie@example.com', 'USER'
                    ) AS tmp
                    WHERE NOT EXISTS (SELECT 1 FROM users LIMIT 1)
                    """;
            stmt.execute(insertSQL);
        }
    }

    // ==================== USER CLASS ====================
    static class User {
        private int id;
        private String username;
        private String fullName;
        private String email;
        private String role;
        private int failedAttempts;
        private Timestamp lockedUntil;

        public User(int id, String username, String fullName, String email,
                String role, int failedAttempts, Timestamp lockedUntil) {
            this.id = id;
            this.username = username;
            this.fullName = fullName;
            this.email = email;
            this.role = role;
            this.failedAttempts = failedAttempts;
            this.lockedUntil = lockedUntil;
        }

        public boolean isLocked() {
            return lockedUntil != null && lockedUntil.after(new Timestamp(System.currentTimeMillis()));
        }
    }

    // ==================== FAILED ATTEMPT TRACKER ====================
    static class FailedAttempt {
        String username;
        int count;
        long firstAttempt;
        long lastAttempt;

        FailedAttempt(String username) {
            this.username = username;
            this.count = 1;
            this.firstAttempt = System.currentTimeMillis();
            this.lastAttempt = System.currentTimeMillis();
        }

        void increment() {
            count++;
            lastAttempt = System.currentTimeMillis();
        }

        boolean shouldLock() {
            return count >= 5 && (lastAttempt - firstAttempt) < 300000; // 5 minutes
        }
    }

    // ==================== INSECURE LOGIN PROCESSOR (CWE-209 VULNERABLE)
    // ====================
    class InsecureLoginProcessor {

        public void processLogin(HttpServletRequest request, HttpServletResponse response)
                throws IOException {

            PrintWriter out = response.getWriter();
            String username = request.getParameter("username");
            String password = request.getParameter("password");

            // INSECURE: Direct database access without proper error handling
            Connection conn = null;
            Statement stmt = null;
            ResultSet rs = null;

            try {
                // INSECURE: Loading driver manually (shows implementation details)
                Class.forName("org.h2.Driver");

                // INSECURE: Connection details exposed in error
                conn = DriverManager.getConnection(DB_URL, DB_USER, DB_PASSWORD);

                // INSECURE: Vulnerable to SQL injection
                String query = "SELECT * FROM users WHERE username = '" + username + "'";
                stmt = conn.createStatement();
                rs = stmt.executeQuery(query);

                if (rs.next()) {
                    String dbPassword = rs.getString("password_hash");

                    // Simple password check (insecure - just for demo)
                    if (password.equals("password") || dbPassword.equals(hashPassword(password))) {
                        out.println("<html><body>");
                        out.println("<h2>Login Successful!</h2>");
                        out.println("<p>Welcome, " + rs.getString("full_name") + "!</p>");
                        out.println("</body></html>");
                    } else {
                        out.println("<html><body>");
                        out.println("<h2>Login Failed</h2>");
                        out.println("<p>Invalid password</p>");
                        out.println("</body></html>");
                    }
                } else {
                    out.println("<html><body>");
                    out.println("<h2>Login Failed</h2>");
                    out.println("<p>User not found: " + username + "</p>");
                    out.println("</body></html>");
                }

            } catch (ClassNotFoundException e) {
                // INSECURE: Exposing full stack trace to user!
                response.setStatus(500);
                out.println("<html><body>");
                out.println("<h2>Database Driver Error</h2>");
                out.println("<pre>");
                e.printStackTrace(out); // CWE-209: Stack trace exposed!
                out.println("</pre>");
                out.println("</body></html>");

            } catch (SQLException e) {
                // INSECURE: Exposing database details to user!
                response.setStatus(500);
                out.println("<html><body>");
                out.println("<h2>Database Error</h2>");
                out.println("<p>Error Code: " + e.getErrorCode() + "</p>");
                out.println("<p>SQL State: " + e.getSQLState() + "</p>");
                out.println("<p>Message: " + e.getMessage() + "</p>");
                out.println("<pre>");
                e.printStackTrace(out); // CWE-209: Stack trace exposed!
                out.println("</pre>");
                out.println("</body></html>");

            } finally {
                try {
                    if (rs != null)
                        rs.close();
                } catch (Exception e) {
                }
                try {
                    if (stmt != null)
                        stmt.close();
                } catch (Exception e) {
                }
                try {
                    if (conn != null)
                        conn.close();
                } catch (Exception e) {
                }
            }
        }
    }

    // ==================== SECURE LOGIN PROCESSOR (CWE-209 MITIGATED)
    // ====================
    class SecureLoginProcessor {

        private static final String USER_AGENT = "User-Agent";
        private static final String X_FORWARDED_FOR = "X-Forwarded-For";

        public void processLogin(HttpServletRequest request, HttpServletResponse response)
                throws IOException {

            PrintWriter out = response.getWriter();
            String username = sanitizeInput(request.getParameter("username"));
            String password = request.getParameter("password"); // Don't sanitize password

            // Validate input
            if (username == null || username.trim().isEmpty() ||
                    password == null || password.trim().isEmpty()) {
                showLoginForm(out, ERROR_INVALID_CREDENTIALS);
                return;
            }

            // Check for brute force attempts
            String clientIp = getClientIp(request);
            if (isRateLimited(clientIp)) {
                LOGGER.warning("Rate limit exceeded for IP: " + clientIp);
                showLoginForm(out, "Too many login attempts. Please try again later.");
                return;
            }

            try (Connection conn = getDatabaseConnection()) {
                User user = authenticateUser(conn, username, password);

                if (user != null) {
                    // Check if account is locked
                    if (user.isLocked()) {
                        LOGGER.info("Locked account access attempt: " + username);
                        showLoginForm(out, ERROR_ACCOUNT_LOCKED);
                        return;
                    }

                    // Successful login
                    handleSuccessfulLogin(conn, user, request);
                    showSuccessPage(out, user);

                } else {
                    // Failed login
                    handleFailedLogin(username, clientIp);
                    showLoginForm(out, ERROR_INVALID_CREDENTIALS);
                }

            } catch (SQLException e) {
                // SECURE: Log full details internally
                LOGGER.log(Level.SEVERE, "Database error during login for user: " + username, e);

                // SECURE: Show generic error to user
                response.setStatus(500);
                showErrorPage(out, ERROR_DB_CONNECTION);

            } catch (Exception e) {
                // SECURE: Catch any unexpected errors
                LOGGER.log(Level.SEVERE, "Unexpected error during login", e);
                response.setStatus(500);
                showErrorPage(out, ERROR_GENERAL);
            }
        }

        private Connection getDatabaseConnection() throws SQLException {
            // SECURE: Use connection pooling in production
            return DriverManager.getConnection(DB_URL, DB_USER, DB_PASSWORD);
        }

        private User authenticateUser(Connection conn, String username, String password)
                throws SQLException {

            // SECURE: Use PreparedStatement to prevent SQL injection
            String sql = "SELECT id, username, full_name, email, role, " +
                    "failed_attempts, locked_until FROM users " +
                    "WHERE username = ? AND password_hash = ?";

            try (PreparedStatement pstmt = conn.prepareStatement(sql)) {
                pstmt.setString(1, username);
                pstmt.setString(2, hashPassword(password)); // In production, use proper password hashing

                try (ResultSet rs = pstmt.executeQuery()) {
                    if (rs.next()) {
                        return new User(
                                rs.getInt("id"),
                                rs.getString("username"),
                                rs.getString("full_name"),
                                rs.getString("email"),
                                rs.getString("role"),
                                rs.getInt("failed_attempts"),
                                rs.getTimestamp("locked_until"));
                    }
                }
            }

            return null;
        }

        private void handleSuccessfulLogin(Connection conn, User user, HttpServletRequest request)
                throws SQLException {

            // Reset failed attempts
            String updateSql = "UPDATE users SET failed_attempts = 0, " +
                    "locked_until = NULL, last_login = ? WHERE id = ?";

            try (PreparedStatement pstmt = conn.prepareStatement(updateSql)) {
                pstmt.setTimestamp(1, new Timestamp(System.currentTimeMillis()));
                pstmt.setInt(2, user.id);
                pstmt.executeUpdate();
            }

            // Create session
            HttpSession session = request.getSession();
            session.setAttribute("userId", user.id);
            session.setAttribute("username", user.username);
            session.setAttribute("role", user.role);
            session.setMaxInactiveInterval(30 * 60); // 30 minutes

            // SECURE: Log successful login (without password)
            LOGGER.info(String.format("Successful login - User: %s, Role: %s, IP: %s",
                    user.username, user.role, getClientIp(request)));
        }

        private void handleFailedLogin(String username, String clientIp) {
            // Track failed attempts
            FailedAttempt attempt = failedAttempts.computeIfAbsent(clientIp,
                    k -> new FailedAttempt(username));
            attempt.increment();

            // Lock account if too many failures
            if (attempt.shouldLock()) {
                lockUserAccount(username);
                LOGGER.warning(String.format("Account locked - User: %s, IP: %s, Attempts: %d",
                        username, clientIp, attempt.count));
            }

            // SECURE: Log failed attempt (without exposing details)
            LOGGER.info(String.format("Failed login attempt - User: %s, IP: %s, Attempts: %d",
                    username, clientIp, attempt.count));
        }

        private void lockUserAccount(String username) {
            String sql = "UPDATE users SET locked_until = ? WHERE username = ?";

            try (Connection conn = getDatabaseConnection();
                    PreparedStatement pstmt = conn.prepareStatement(sql)) {

                Timestamp lockUntil = new Timestamp(System.currentTimeMillis() + 30 * 60 * 1000); // 30 minutes
                pstmt.setTimestamp(1, lockUntil);
                pstmt.setString(2, username);
                pstmt.executeUpdate();

            } catch (SQLException e) {
                LOGGER.log(Level.SEVERE, "Failed to lock account: " + username, e);
            }
        }

        private boolean isRateLimited(String clientIp) {
            FailedAttempt attempt = failedAttempts.get(clientIp);
            if (attempt != null) {
                return attempt.count >= 10; // Max 10 attempts
            }
            return false;
        }

        private String getClientIp(HttpServletRequest request) {
            String ip = request.getHeader(X_FORWARDED_FOR);
            if (ip == null || ip.isEmpty()) {
                ip = request.getRemoteAddr();
            }
            return ip;
        }

        private String sanitizeInput(String input) {
            if (input == null)
                return null;
            // Remove potentially dangerous characters
            return input.replaceAll("[<>\"'%;()&+]", "");
        }
    }

    // ==================== HELPER METHODS ====================

    private static String hashPassword(String password) {
        // Simple hash for demo - use BCrypt in production
        try {
            java.security.MessageDigest md = java.security.MessageDigest.getInstance("MD5");
            byte[] hash = md.digest(password.getBytes("UTF-8"));
            StringBuilder hexString = new StringBuilder();
            for (byte b : hash) {
                String hex = Integer.toHexString(0xff & b);
                if (hex.length() == 1)
                    hexString.append('0');
                hexString.append(hex);
            }
            return hexString.toString();
        } catch (Exception e) {
            return password;
        }
    }

    private void showLoginForm(PrintWriter out, String error) {
        out.println("<!DOCTYPE html>");
        out.println("<html>");
        out.println("<head>");
        out.println("    <title>Login - MyApp</title>");
        out.println("    <style>");
        out.println(
                "        body { font-family: Arial, sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }");
        out.println(
                "        .login-container { background: white; padding: 40px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); width: 300px; }");
        out.println("        h2 { text-align: center; color: #333; margin-bottom: 30px; }");
        out.println(
                "        .error { background: #ffebee; color: #c62828; padding: 10px; border-radius: 4px; margin-bottom: 20px; font-size: 14px; }");
        out.println(
                "        input { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }");
        out.println(
                "        button { width: 100%; padding: 10px; background: #1877f2; color: white; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; }");
        out.println("        button:hover { background: #166fe5; }");
        out.println("    </style>");
        out.println("</head>");
        out.println("<body>");
        out.println("    <div class='login-container'>");
        out.println("        <h2>MyApp Login</h2>");

        if (error != null && !error.isEmpty()) {
            out.println("        <div class='error'>" + error + "</div>");
        }

        out.println("        <form method='POST' action='login'>");
        out.println("            <input type='text' name='username' placeholder='Username' required>");
        out.println("            <input type='password' name='password' placeholder='Password' required>");
        out.println("            <button type='submit'>Log In</button>");
        out.println("        </form>");
        out.println("    </div>");
        out.println("</body>");
        out.println("</html>");
    }

    private void showSuccessPage(PrintWriter out, User user) {
        out.println("<!DOCTYPE html>");
        out.println("<html>");
        out.println("<head>");
        out.println("    <title>Welcome - MyApp</title>");
        out.println("    <style>");
        out.println("        body { font-family: Arial, sans-serif; background: #f0f2f5; margin: 0; padding: 20px; }");
        out.println(
                "        .container { max-width: 800px; margin: 0 auto; background: white; padding: 30px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }");
        out.println("        h1 { color: #333; }");
        out.println("        .user-info { background: #f5f5f5; padding: 20px; border-radius: 4px; margin: 20px 0; }");
        out.println("        .info-item { margin: 10px 0; color: #666; }");
        out.println("        .label { font-weight: bold; color: #333; }");
        out.println(
                "        .logout { display: inline-block; padding: 10px 20px; background: #1877f2; color: white; text-decoration: none; border-radius: 4px; }");
        out.println("    </style>");
        out.println("</head>");
        out.println("<body>");
        out.println("    <div class='container'>");
        out.println("        <h1>Welcome, " + user.fullName + "!</h1>");
        out.println("        <div class='user-info'>");
        out.println(
                "            <div class='info-item'><span class='label'>Username:</span> " + user.username + "</div>");
        out.println("            <div class='info-item'><span class='label'>Email:</span> " + user.email + "</div>");
        out.println("            <div class='info-item'><span class='label'>Role:</span> " + user.role + "</div>");
        out.println("        </div>");
        out.println("        <a href='logout' class='logout'>Logout</a>");
        out.println("    </div>");
        out.println("</body>");
        out.println("</html>");
    }

    private void showErrorPage(PrintWriter out, String message) {
        out.println("<!DOCTYPE html>");
        out.println("<html>");
        out.println("<head>");
        out.println("    <title>Error - MyApp</title>");
        out.println("    <style>");
        out.println(
                "        body { font-family: Arial, sans-serif; background: #f0f2f5; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }");
        out.println(
                "        .error-container { background: white; padding: 40px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); text-align: center; max-width: 400px; }");
        out.println("        h2 { color: #c62828; margin-bottom: 20px; }");
        out.println("        p { color: #666; margin-bottom: 30px; }");
        out.println(
                "        .button { display: inline-block; padding: 10px 20px; background: #1877f2; color: white; text-decoration: none; border-radius: 4px; }");
        out.println("        .button:hover { background: #166fe5; }");
        out.println("    </style>");
        out.println("</head>");
        out.println("<body>");
        out.println("    <div class='error-container'>");
        out.println("        <h2>Something Went Wrong</h2>");
        out.println("        <p>" + message + "</p>");
        out.println("        <a href='login' class='button'>Back to Login</a>");
        out.println("    </div>");
        out.println("</body>");
        out.println("</html>");
    }

    // ==================== SERVLET METHODS ====================

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        response.setContentType("text/html;charset=UTF-8");
        PrintWriter out = response.getWriter();
        showLoginForm(out, null);
    }

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {

        response.setContentType("text/html;charset=UTF-8");

        // Get mode from request parameter (for demonstration)
        String mode = request.getParameter("mode");

        if ("insecure".equals(mode)) {
            // Use insecure processor (CWE-209 vulnerable)
            InsecureLoginProcessor insecureProcessor = new InsecureLoginProcessor();
            insecureProcessor.processLogin(request, response);
        } else {
            // Use secure processor (default)
            SecureLoginProcessor secureProcessor = new SecureLoginProcessor();
            secureProcessor.processLogin(request, response);
        }
    }

    @Override
    public String getServletInfo() {
        return "CWE-209: Information Exposure Through Error Message Demo";
    }
}