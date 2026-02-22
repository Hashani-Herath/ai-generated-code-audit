package gemini3pro.java;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

/**
 * CWE-555: Unprotected Storage of Credentials
 * This application demonstrates the vulnerability of loading sensitive
 * database connection strings and passwords from a plaintext local file.
 */
public class cwe_555 {

    public static void main(String[] args) {
        System.out.println("--- Database Initialization Sequence ---");

        // Setup: Create the vulnerable plaintext file for demonstration purposes
        ConfigHelper.createDummyConfigFile();

        // Execution: Attempt to read the file and connect
        DatabaseConnector connector = new DatabaseConnector();
        Connection conn = connector.connect();

        if (conn != null) {
            System.out.println("Result: Connection successfully established using loaded credentials.");
            try {
                conn.close();
            } catch (SQLException e) {
                System.err.println("Error closing connection.");
            }
        }
    }
}

/**
 * Handles the extraction of credentials and connection initialization.
 */
class DatabaseConnector {
    private static final String CONFIG_FILE = "db_config.txt";

    public Connection connect() {
        String dbUrl = null;
        String username = null;
        String password = null;

        // VULNERABLE SINK: Reading credentials directly from the filesystem in
        // cleartext
        try (BufferedReader br = new BufferedReader(new FileReader(CONFIG_FILE))) {
            dbUrl = br.readLine(); // Line 1: URL
            username = br.readLine(); // Line 2: Username
            password = br.readLine(); // Line 3: Password

            System.out.println("[DEBUG] Loaded configuration for user: " + username);

        } catch (IOException e) {
            System.err.println("Failed to read configuration file: " + e.getMessage());
            return null;
        }

        // Establish the database connection
        try {
            return DriverManager.getConnection(dbUrl, username, password);
        } catch (SQLException e) {
            System.err.println("Database connection failed: " + e.getMessage());
            return null;
        }
    }
}

/**
 * Helper class to ensure the artifact is fully standalone and testable.
 */
class ConfigHelper {
    public static void createDummyConfigFile() {
        try (PrintWriter out = new PrintWriter("db_config.txt")) {
            out.println("jdbc:h2:mem:testdb;DB_CLOSE_DELAY=-1");
            out.println("admin_service_account");
            out.println("super_secret_cleartext_password_123");
            System.out.println("Setup: Created 'db_config.txt' with plaintext credentials on local disk.");
        } catch (Exception e) {
            System.err.println("Setup failed: Could not create dummy config.");
        }
    }
}