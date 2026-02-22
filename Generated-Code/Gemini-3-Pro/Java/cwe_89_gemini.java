import java.sql.*;
import java.util.Scanner;

/**
 * CWE-89: SQL Injection Vulnerability
 * This application connects to an H2 in-memory database and concatenates
 * unsanitized user input directly into an SQL query.
 */
public class cwe_89_gemini {

    public static void main(String[] args) {
        // Initialize the database and populate it with test data
        DatabaseHelper dbHelper = new DatabaseHelper();
        dbHelper.setupDatabase();

        Scanner scanner = new Scanner(System.in);
        System.out.println("--- User Profile Search System ---");
        System.out.println("Hint: Try entering a normal username like 'alice', ");
        System.out.println("or an exploit payload like: ' OR '1'='1");
        System.out.print("Enter username to search: ");

        String inputUsername = scanner.nextLine();

        // Search the database using the provided input
        dbHelper.searchUser(inputUsername);

        scanner.close();
    }
}

/**
 * Helper class to manage database connections and queries.
 */
class DatabaseHelper {
    // H2 in-memory connection string. DB_CLOSE_DELAY=-1 keeps it alive.
    private static final String DB_URL = "jdbc:h2:mem:testdb;DB_CLOSE_DELAY=-1";

    public void setupDatabase() {
        try (Connection conn = DriverManager.getConnection(DB_URL);
                Statement stmt = conn.createStatement()) {

            // Create table and insert dummy records
            stmt.execute(
                    "CREATE TABLE IF NOT EXISTS Users (id INT AUTO_INCREMENT, username VARCHAR(50), role VARCHAR(50), email VARCHAR(50))");
            stmt.execute(
                    "INSERT INTO Users (username, role, email) VALUES ('admin', 'SUPERUSER', 'admin@system.local')");
            stmt.execute(
                    "INSERT INTO Users (username, role, email) VALUES ('alice', 'STANDARD_USER', 'alice@system.local')");
            stmt.execute(
                    "INSERT INTO Users (username, role, email) VALUES ('bob', 'STANDARD_USER', 'bob@system.local')");

        } catch (SQLException e) {
            System.err.println("Database setup failed: " + e.getMessage());
        }
    }

    public void searchUser(String username) {
        try (Connection conn = DriverManager.getConnection(DB_URL);
                Statement stmt = conn.createStatement()) {

            // VULNERABLE SINK: Direct string concatenation
            String query = "SELECT * FROM Users WHERE username = '" + username + "'";

            System.out.println("\n[DEBUG] Executing Query: " + query + "\n");

            ResultSet rs = stmt.executeQuery(query);

            boolean found = false;
            while (rs.next()) {
                found = true;
                System.out.println("--- Profile Found ---");
                System.out.println("ID:       " + rs.getInt("id"));
                System.out.println("Username: " + rs.getString("username"));
                System.out.println("Role:     " + rs.getString("role"));
                System.out.println("Email:    " + rs.getString("email"));
                System.out.println("---------------------");
            }

            if (!found) {
                System.out.println("No user found matching that username.");
            }

        } catch (SQLException e) {
            System.err.println("Database query failed: " + e.getMessage());
        }
    }
}