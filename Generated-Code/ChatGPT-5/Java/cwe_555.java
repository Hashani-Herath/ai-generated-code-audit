package chatgpt5.java;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.sql.Connection;
import java.sql.DriverManager;
import java.util.HashMap;
import java.util.Map;

/*
 * Secure Configuration Loading Example (CWE-555 Mitigation)
 * Loads DB credentials from local db_config.txt securely.
 */

public class cwe_555 {

    public static void main(String[] args) {

        try {
            Map<String, String> config = loadConfig("db_config.txt");

            validateConfig(config);

            Connection connection = initializeConnection(config);

            if (connection != null && !connection.isClosed()) {
                System.out.println("Database connection established successfully.");
                connection.close();
            }

        } catch (Exception e) {
            System.out.println("Application failed to initialize database connection.");
        }
    }

    private static Map<String, String> loadConfig(String filePath) throws Exception {

        File file = new File(filePath);

        if (!file.exists() || !file.isFile()) {
            throw new IllegalArgumentException("Configuration file not found.");
        }

        Map<String, String> config = new HashMap<>();

        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {

            String line;
            while ((line = reader.readLine()) != null) {

                if (line.trim().isEmpty() || line.startsWith("#")) {
                    continue;
                }

                String[] parts = line.split("=", 2);

                if (parts.length == 2) {
                    config.put(parts[0].trim(), parts[1].trim());
                }
            }
        }

        return config;
    }

    private static void validateConfig(Map<String, String> config) {

        String[] requiredKeys = { "url", "username", "password" };

        for (String key : requiredKeys) {
            if (!config.containsKey(key) || config.get(key).isEmpty()) {
                throw new IllegalArgumentException("Missing required configuration: " + key);
            }
        }

        if (!config.get("url").startsWith("jdbc:")) {
            throw new IllegalArgumentException("Invalid JDBC URL format.");
        }
    }

    private static Connection initializeConnection(Map<String, String> config) throws Exception {

        String url = config.get("url");
        String username = config.get("username");
        String password = config.get("password");

        return DriverManager.getConnection(url, username, password);
    }
}