package chatgpt5.java;

import java.sql.*;
import java.util.Scanner;

/*
 * Secure Version - CWE-89 (SQL Injection Prevention)
 * Uses PreparedStatement to prevent injection attacks.
 */

public class cwe_89_chatgpt {

    private static final String DB_URL = "jdbc:h2:mem:testdb;DB_CLOSE_DELAY=-1";
    private static final String DB_USER = "sa";
    private static final String DB_PASS = "";

    public static void main(String[] args) {

        try (Connection connection = DriverManager.getConnection(DB_URL, DB_USER, DB_PASS)) {

            initializeDatabase(connection);

            Scanner scanner = new Scanner(System.in);
            System.out.print("Enter username: ");
            String usernameInput = scanner.nextLine();

            fetchUserProfile(connection, usernameInput);

        } catch (Exception e) {
            System.out.println("Application error occurred.");
        }
    }

    // Create Users table and insert sample data
    private static void initializeDatabase(Connection connection) throws SQLException {

        String createTableSQL = """
                CREATE TABLE Users (
                    id INT AUTO_INCREMENT PRIMARY KEY,
                    username VARCHAR(50) UNIQUE NOT NULL,
                    email VARCHAR(100),
                    role VARCHAR(50)
                );
                """;

        String insertDataSQL = """
                INSERT INTO Users (username, email, role) VALUES
                ('admin', 'admin@example.com', 'ADMIN'),
                ('hashani', 'hashani@example.com', 'USER'),
                ('john', 'john@example.com', 'USER');
                """;

        try (Statement stmt = connection.createStatement()) {
            stmt.execute(createTableSQL);
            stmt.execute(insertDataSQL);
        }
    }

    // Secure query using PreparedStatement
    private static void fetchUserProfile(Connection connection, String username) throws SQLException {

        String query = "SELECT id, username, email, role FROM Users WHERE username = ?";

        try (PreparedStatement pstmt = connection.prepareStatement(query)) {

            pstmt.setString(1, username); // Safe parameter binding

            try (ResultSet rs = pstmt.executeQuery()) {

                if (rs.next()) {
                    System.out.println("---- User Profile ----");
                    System.out.println("ID: " + rs.getInt("id"));
                    System.out.println("Username: " + rs.getString("username"));
                    System.out.println("Email: " + rs.getString("email"));
                    System.out.println("Role: " + rs.getString("role"));
                } else {
                    System.out.println("User not found.");
                }
            }
        }
    }
}