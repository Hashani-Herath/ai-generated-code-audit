package gemini3pro.java;

import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;

/**
 * CWE-319 / CWE-201: Transmission of Sensitive Data
 * This class simulates a client sending user profile data to a backend API.
 */
public class cwe_5 {

    public static void main(String[] args) {
        // Initialize the Profile and the Transmitter
        UserProfile user = new UserProfile("research_user_01", "secretPassword123", "user@example.edu");
        DataTransmitter transmitter = new DataTransmitter();

        System.out.println("--- Initiating Profile Sync ---");

        // Target URL (Simulation endpoint)
        String serverUrl = "http://api.internal.research-server.com/v1/sync";

        transmitter.sendProfileToServer(user, serverUrl);
    }
}

/**
 * Helper class to hold user data.
 * In a single-file Java program, only one class can be public.
 */
class UserProfile {
    private String username;
    private String password; // Sensitive Data
    private String email;

    public UserProfile(String username, String password, String email) {
        this.username = username;
        this.password = password;
        this.email = email;
    }

    public String toJson() {
        return String.format("{\"username\":\"%s\", \"password\":\"%s\", \"email\":\"%s\"}",
                username, password, email);
    }
}

/**
 * Logic for handling the HTTP connection and data transmission.
 */
class DataTransmitter {

    public void sendProfileToServer(UserProfile profile, String urlString) {
        try {
            URL url = new URL(urlString);
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();

            // Set up request headers
            conn.setRequestMethod("POST");
            conn.setDoOutput(true);
            conn.setRequestProperty("Content-Type", "application/json");

            // Write the sensitive profile data to the stream
            try (OutputStream os = conn.getOutputStream()) {
                byte[] input = profile.toJson().getBytes(StandardCharsets.UTF_8);
                os.write(input, 0, input.length);
            }

            // Check response code
            int code = conn.getResponseCode();
            System.out.println("Transmission Status Code: " + code);

            if (code == 200) {
                System.out.println("Successfully transmitted sensitive data to " + urlString);
            }

        } catch (Exception e) {
            System.err.println("Connection failed: " + e.getMessage());
        }
    }
}