import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;

/*
 * CWE-5 Demonstration
 * Java client transmitting sensitive profile data via HTTP request.
 * This example intentionally uses HTTP (not HTTPS) for demonstration.
 */

public class cwe_5 {

    public static void main(String[] args) {

        // Create user profile with sensitive information
        UserProfile profile = new UserProfile(
                "Hashani Herath",
                "hashani@example.com",
                "0771234567",
                "123456789V", // National ID
                "SuperSecret123" // Password
        );

        ProfileClient client = new ProfileClient();
        client.sendProfile(profile);
    }
}

/*
 * Represents sensitive user profile data
 */
class UserProfile {

    private String fullName;
    private String email;
    private String phoneNumber;
    private String nationalId;
    private String password;

    public UserProfile(String fullName, String email, String phoneNumber,
            String nationalId, String password) {
        this.fullName = fullName;
        this.email = email;
        this.phoneNumber = phoneNumber;
        this.nationalId = nationalId;
        this.password = password;
    }

    // Convert profile data to JSON format
    public String toJson() {
        return "{"
                + "\"fullName\":\"" + fullName + "\","
                + "\"email\":\"" + email + "\","
                + "\"phoneNumber\":\"" + phoneNumber + "\","
                + "\"nationalId\":\"" + nationalId + "\","
                + "\"password\":\"" + password + "\""
                + "}";
    }
}

/*
 * Client responsible for sending profile data to server
 */
class ProfileClient {

    // Example central server URL (HTTP - insecure)
    private static final String SERVER_URL = "http://example.com/api/profile";

    public void sendProfile(UserProfile profile) {

        try {
            URL url = new URL(SERVER_URL);
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();

            connection.setRequestMethod("POST");
            connection.setDoOutput(true);
            connection.setRequestProperty("Content-Type", "application/json");

            String jsonPayload = profile.toJson();

            // Send data to server
            try (OutputStream os = connection.getOutputStream()) {
                byte[] input = jsonPayload.getBytes(StandardCharsets.UTF_8);
                os.write(input, 0, input.length);
            }

            int responseCode = connection.getResponseCode();
            System.out.println("Response Code: " + responseCode);

            connection.disconnect();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}