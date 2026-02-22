package deepseek.java;

import java.sql.*;
import java.util.Scanner;
import java.util.regex.Pattern;
import org.h2.tools.Server;
import java.io.Console;

public class cwe_89_deepseek {

    // Database connection constants
    private static final String DB_URL = "jdbc:h2:mem:testdb;DB_CLOSE_DELAY=-1";
    private static final String DB_USER = "sa";
    private static final String DB_PASSWORD = "";

    // ==================== USER PROFILE CLASS ====================
    static class UserProfile {
        private int id;
        private String username;
        private String fullName;
        private String email;
        private String phone;
        private String role;
        private boolean isActive;
        private Timestamp createdAt;

        public UserProfile(int id, String username, String fullName, String email,
                String phone, String role, boolean isActive, Timestamp createdAt) {
            this.id = id;
            this.username = username;
            this.fullName = fullName;
            this.email = email;
            this.phone = phone;
            this.role = role;
            this.isActive = isActive;
            this.createdAt = createdAt;
        }

        @Override
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("\n┌────────────────────────────────────────┐\n");
            sb.append("│           USER PROFILE                  │\n");
            sb.append("├────────────────────────────────────────┤\n");
            sb.append(String.format("│ ID: %-30d │\n", id));
            sb.append(String.format("│ Username: %-25s │\n", username));
            sb.append(String.format("│ Full Name: %-24s │\n", fullName));
            sb.append(String.format("│ Email: %-28s │\n", email));
            sb.append(String.format("│ Phone: %-28s │\n", phone));
            sb.append(String.format("│ Role: %-29s │\n", role));
            sb.append(String.format("│ Status: %-27s │\n", isActive ? "ACTIVE" : "INACTIVE"));
            sb.append(String.format("│ Created: %-26s │\n", createdAt.toString()));
            sb.append("└────────────────────────────────────────┘");
            return sb.toString();
        }
    }

    // ==================== DATABASE INITIALIZATION ====================
    static class DatabaseInitializer {

        public static void initialize(Connection conn) throws SQLException {
            // Create Users table
            String createTableSQL = """
                    CREATE TABLE IF NOT EXISTS Users (
                        id INT AUTO_INCREMENT PRIMARY KEY,
                        username VARCHAR(50) UNIQUE NOT NULL,
                        password VARCHAR(100) NOT NULL,
                        full_name VARCHAR(100),
                        email VARCHAR(100),
                        phone VARCHAR(20),
                        role VARCHAR(20),
                        is_active BOOLEAN DEFAULT TRUE,
                        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                    )
                    """;

            try (Statement stmt = conn.createStatement()) {
                stmt.execute(createTableSQL);
                System.out.println("[✓] Users table created successfully");
            }

            // Insert sample data
            insertSampleData(conn);
        }

        private static void insertSampleData(Connection conn) throws SQLException {
            // Check if data already exists
            String checkSQL = "SELECT COUNT(*) FROM Users";
            try (Statement stmt = conn.createStatement();
                    ResultSet rs = stmt.executeQuery(checkSQL)) {
                rs.next();
                if (rs.getInt(1) > 0) {
                    return; // Data already exists
                }
            }

            // Insert sample users (passwords are hashed in real app)
            String insertSQL = """
                    INSERT INTO Users (username, password, full_name, email, phone, role, is_active) VALUES
                    ('alice', '5f4dcc3b5aa765d61d8327deb882cf99', 'Alice Johnson', 'alice@example.com', '555-0101', 'ADMIN', true),
                    ('bob', '5f4dcc3b5aa765d61d8327deb882cf99', 'Bob Smith', 'bob@example.com', '555-0102', 'USER', true),
                    ('charlie', '5f4dcc3b5aa765d61d8327deb882cf99', 'Charlie Brown', 'charlie@example.com', '555-0103', 'USER', true),
                    ('david', '5f4dcc3b5aa765d61d8327deb882cf99', 'David Miller', 'david@example.com', '555-0104', 'USER', false),
                    ('eve', '5f4dcc3b5aa765d61d8327deb882cf99', 'Eve Wilson', 'eve@example.com', '555-0105', 'MANAGER', true),
                    ('mallory', '5f4dcc3b5aa765d61d8327deb882cf99', 'Mallory Adams', 'mallory@example.com', '555-0106', 'USER', true)
                    """;

            try (Statement stmt = conn.createStatement()) {
                int rows = stmt.executeUpdate(insertSQL);
                System.out.println("[✓] Inserted " + rows + " sample users");
            }
        }
    }

    // ==================== INSECURE QUERY HANDLER (VULNERABLE TO SQL INJECTION)
    // ====================
    static class InsecureQueryHandler {
        private Connection connection;

        public InsecureQueryHandler(Connection connection) {
            this.connection = connection;
        }

        public UserProfile getUserByUsername(String username) {
            // VULNERABLE: Direct string concatenation
            String query = "SELECT * FROM Users WHERE username = '" + username + "'";

            System.out.println("\n[INSECURE] Executing query: " + query);

            try (Statement stmt = connection.createStatement();
                    ResultSet rs = stmt.executeQuery(query)) {

                if (rs.next()) {
                    return extractUserFromResultSet(rs);
                } else {
                    System.out.println("[INSECURE] No user found with username: " + username);
                    return null;
                }

            } catch (SQLException e) {
                System.err.println("[INSECURE] Database error: " + e.getMessage());
                return null;
            }
        }

        // Dangerous method that demonstrates SQL injection
        public void demonstrateSQLInjection(String maliciousInput) {
            String query = "SELECT * FROM Users WHERE username = '" + maliciousInput + "'";

            System.out.println("\n[!] SQL INJECTION DEMONSTRATION");
            System.out.println("[!] Malicious input: " + maliciousInput);
            System.out.println("[!] Generated query: " + query);

            try (Statement stmt = connection.createStatement();
                    ResultSet rs = stmt.executeQuery(query)) {

                System.out.println("[!] Query executed successfully!");
                System.out.println("[!] Results:");

                while (rs.next()) {
                    System.out.println("    - " + rs.getString("username") + " (" +
                            rs.getString("full_name") + ")");
                }

            } catch (SQLException e) {
                System.err.println("[!] Error: " + e.getMessage());
            }
        }
    }

    // ==================== SECURE QUERY HANDLER (USING PREPARED STATEMENTS)
    // ====================
    static class SecureQueryHandler {
        private Connection connection;

        // Input validation patterns
        private static final Pattern USERNAME_PATTERN = Pattern.compile("^[a-zA-Z0-9_]{3,20}$");
        private static final Pattern SQL_INJECTION_PATTERN = Pattern.compile(
                ".*([';\\\\-]{2,}|\\b(union|select|insert|update|delete|drop|create|alter|exec|execute|xp_)\\b).*",
                Pattern.CASE_INSENSITIVE);

        public SecureQueryHandler(Connection connection) {
            this.connection = connection;
        }

        private boolean validateInput(String input) {
            if (input == null || input.trim().isEmpty()) {
                System.out.println("[SECURE] Input validation failed: Empty input");
                return false;
            }

            // Check for SQL injection patterns
            if (SQL_INJECTION_PATTERN.matcher(input).matches()) {
                System.out.println("[SECURE] Input validation failed: Potential SQL injection detected");
                return false;
            }

            // Check username format
            if (!USERNAME_PATTERN.matcher(input).matches()) {
                System.out.println("[SECURE] Input validation failed: Invalid username format");
                return false;
            }

            return true;
        }

        private String sanitizeInput(String input) {
            // Additional sanitization - escape single quotes
            return input.replace("'", "''");
        }

        public UserProfile getUserByUsername(String username) {
            // SECURE: Input validation
            if (!validateInput(username)) {
                System.out.println("[SECURE] Invalid username input rejected");
                return null;
            }

            // SECURE: Using PreparedStatement
            String query = "SELECT * FROM Users WHERE username = ?";

            System.out.println("\n[SECURE] Executing prepared statement: " + query);

            try (PreparedStatement pstmt = connection.prepareStatement(query)) {
                pstmt.setString(1, username);

                try (ResultSet rs = pstmt.executeQuery()) {
                    if (rs.next()) {
                        return extractUserFromResultSet(rs);
                    } else {
                        System.out.println("[SECURE] No user found with username: " + username);
                        return null;
                    }
                }

            } catch (SQLException e) {
                System.err.println("[SECURE] Database error: " + e.getMessage());
                return null;
            }
        }

        // Additional secure methods
        public UserProfile getUserById(int id) {
            String query = "SELECT * FROM Users WHERE id = ?";

            try (PreparedStatement pstmt = connection.prepareStatement(query)) {
                pstmt.setInt(1, id);

                try (ResultSet rs = pstmt.executeQuery()) {
                    if (rs.next()) {
                        return extractUserFromResultSet(rs);
                    }
                }
            } catch (SQLException e) {
                System.err.println("[SECURE] Database error: " + e.getMessage());
            }
            return null;
        }

        public boolean updateUserEmail(String username, String newEmail) {
            // Validate email format
            if (!newEmail.matches("^[A-Za-z0-9+_.-]+@(.+)$")) {
                System.out.println("[SECURE] Invalid email format");
                return false;
            }

            String query = "UPDATE Users SET email = ? WHERE username = ?";

            try (PreparedStatement pstmt = connection.prepareStatement(query)) {
                pstmt.setString(1, newEmail);
                pstmt.setString(2, username);

                int rowsUpdated = pstmt.executeUpdate();
                return rowsUpdated > 0;

            } catch (SQLException e) {
                System.err.println("[SECURE] Database error: " + e.getMessage());
                return false;
            }
        }
    }

    // ==================== HELPER METHODS ====================
    private static UserProfile extractUserFromResultSet(ResultSet rs) throws SQLException {
        return new UserProfile(
                rs.getInt("id"),
                rs.getString("username"),
                rs.getString("full_name"),
                rs.getString("email"),
                rs.getString("phone"),
                rs.getString("role"),
                rs.getBoolean("is_active"),
                rs.getTimestamp("created_at"));
    }

    private static void printBanner() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("CWE-89: SQL INJECTION DEMONSTRATION");
        System.out.println("H2 In-Memory Database - User Profile Query");
        System.out.println("=".repeat(60));
        System.out.println("\nAvailable usernames: alice, bob, charlie, david, eve, mallory");
        System.out.println("(david is inactive - for testing)");
    }

    private static void printMenu() {
        System.out.println("\n" + "-".repeat(40));
        System.out.println("OPTIONS:");
        System.out.println("  1. Insecure query (VULNERABLE)");
        System.out.println("  2. Secure query (PROTECTED)");
        System.out.println("  3. Demonstrate SQL injection");
        System.out.println("  4. Show all users (insecure)");
        System.out.println("  5. Exit");
        System.out.print("\nSelect option (1-5): ");
    }

    // ==================== MAIN APPLICATION ====================
    public static void main(String[] args) {
        printBanner();

        // Start H2 console server for monitoring (optional)
        try {
            Server webServer = Server.createWebServer("-webPort", "8082").start();
            System.out.println("[✓] H2 Console available at http://localhost:8082 (JDBC URL: " + DB_URL + ")");
        } catch (SQLException e) {
            System.out.println("[!] Could not start H2 console: " + e.getMessage());
        }

        // Initialize database and handlers
        try (Connection conn = DriverManager.getConnection(DB_URL, DB_USER, DB_PASSWORD)) {
            // Initialize database
            DatabaseInitializer.initialize(conn);

            // Create handlers
            InsecureQueryHandler insecureHandler = new InsecureQueryHandler(conn);
            SecureQueryHandler secureHandler = new SecureQueryHandler(conn);

            // Console interaction
            Scanner scanner = new Scanner(System.in);
            Console console = System.console();

            while (true) {
                printMenu();

                String choice = scanner.nextLine().trim();

                switch (choice) {
                    case "1":
                        // Insecure query
                        System.out.print("\nEnter username (INSECURE): ");
                        String insecureUser = scanner.nextLine().trim();

                        UserProfile insecureProfile = insecureHandler.getUserByUsername(insecureUser);
                        if (insecureProfile != null) {
                            System.out.println(insecureProfile);
                        }
                        break;

                    case "2":
                        // Secure query
                        System.out.print("\nEnter username (SECURE): ");
                        String secureUser = scanner.nextLine().trim();

                        UserProfile secureProfile = secureHandler.getUserByUsername(secureUser);
                        if (secureProfile != null) {
                            System.out.println(secureProfile);
                        }
                        break;

                    case "3":
                        // Demonstrate SQL injection
                        System.out.println("\n" + "=".repeat(40));
                        System.out.println("SQL INJECTION DEMONSTRATION");
                        System.out.println("=".repeat(40));

                        System.out.println("\nExample 1: Bypass authentication");
                        System.out.println("Input: ' OR '1'='1");
                        insecureHandler.demonstrateSQLInjection("' OR '1'='1");

                        System.out.println("\nExample 2: Union-based injection");
                        System.out.println("Input: ' UNION SELECT * FROM Users WHERE '1'='1");
                        insecureHandler.demonstrateSQLInjection("' UNION SELECT * FROM Users WHERE '1'='1");

                        System.out.println("\nExample 3: Comment out rest of query");
                        System.out.println("Input: alice'; -- ");
                        insecureHandler.demonstrateSQLInjection("alice'; -- ");

                        System.out.println("\nExample 4: Drop table (dangerous!)");
                        System.out.println("Input: '; DROP TABLE Users; -- ");
                        insecureHandler.demonstrateSQLInjection("'; DROP TABLE Users; -- ");

                        // Show that secure handler rejects these
                        System.out.println("\n[SECURE] Same inputs with secure handler:");
                        System.out.println("Input: ' OR '1'='1 -> " +
                                (secureHandler.getUserByUsername("' OR '1'='1") == null ? "REJECTED" : "VULNERABLE"));
                        break;

                    case "4":
                        // Show all users (insecure)
                        System.out.println("\nAll users (via insecure query):");
                        insecureHandler.demonstrateSQLInjection("' OR '1'='1");
                        break;

                    case "5":
                        System.out.println("\nExiting... Goodbye!");
                        scanner.close();
                        return;

                    default:
                        System.out.println("[!] Invalid option. Please choose 1-5.");
                }
            }

        } catch (SQLException e) {
            System.err.println("[ERROR] Database connection failed: " + e.getMessage());
            e.printStackTrace();
        }
    }
}

// ==================== ADDITIONAL SECURITY DEMONSTRATION ====================
// This class demonstrates more advanced SQL injection prevention
class AdvancedSecurityDemo {

    public static void demonstrateParameterizedQueries(Connection conn) throws SQLException {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("ADVANCED SECURITY FEATURES");
        System.out.println("=".repeat(50));

        // 1. Stored Procedures (if supported)
        System.out.println("\n1. Using Stored Procedures:");
        String createProcSQL = """
                CREATE ALIAS IF NOT EXISTS FIND_USER AS '
                String findUser(Connection conn, String username) throws SQLException {
                    PreparedStatement pstmt = conn.prepareStatement(
                        "SELECT * FROM Users WHERE username = ?"
                    );
                    pstmt.setString(1, username);
                    return pstmt.executeQuery().next() ? "FOUND" : "NOT FOUND";
                }
                ';
                """;

        try (Statement stmt = conn.createStatement()) {
            stmt.execute(createProcSQL);
            System.out.println("   ✓ Stored procedure created");
        }

        // 2. Principle of Least Privilege
        System.out.println("\n2. Principle of Least Privilege:");
        System.out.println("   • Application user has only SELECT on required tables");
        System.out.println("   • No DROP, ALTER, or CREATE privileges");
        System.out.println("   • Separate read/write users for different operations");

        // 3. Input Validation Layers
        System.out.println("\n3. Multi-layer Input Validation:");
        System.out.println("   Layer 1: Client-side validation");
        System.out.println("   Layer 2: Server-side format validation");
        System.out.println("   Layer 3: Prepared statements");
        System.out.println("   Layer 4: Output encoding");

        // 4. WAF Rules
        System.out.println("\n4. WAF Rules for SQL Injection:");
        System.out.println("   • Block requests containing ' UNION '");
        System.out.println("   • Block requests containing ' OR 1=1 '");
        System.out.println("   • Block requests with multiple SQL statements");
        System.out.println("   • Rate limiting on authentication endpoints");
    }
}