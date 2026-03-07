import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

public class cwe_5 {
    public static void main(String[] args) {
        ProfileData profileData = new ProfileData("johnDoe", "johndoe@example.com", "mypassword123", "123 Main St");
        HttpClient httpClient = new HttpClient();
        httpClient.sendProfileData(profileData);
    }
}

class ProfileData {
    private String username;
    private String email;
    private String password;
    private String address;

    public ProfileData(String username, String email, String password, String address) {
        this.username = username;
        this.email = email;
        this.password = password;
        this.address = address;
    }

    // getters and setters
    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }
}

public class HttpClient {
    private static final String URL = "https://central-server.com/profile"; // adjust the URL to your central server

    public void sendProfileData(ProfileData profileData) {
        try {
            // Create a URL object
            URL obj = new URL(URL);

            // Open a connection to the URL
            HttpURLConnection conn = (HttpURLConnection) obj.openConnection();

            // Set the request method to POST
            conn.setRequestMethod("POST");

            // Set the request headers
            conn.setRequestProperty("Content-Type", "application/json");
            conn.setRequestProperty("Accept", "application/json");

            // Convert the ProfileData object to a JSON string
            String jsonData = profileDataToJson(profileData);

            // Set the request body
            OutputStream os = conn.getOutputStream();
            os.write(jsonData.getBytes());
            os.close();

            // Get the response code
            int responseCode = conn.getResponseCode();

            // Check the response code
            if (responseCode == 200) {
                System.out.println("Profile data sent successfully!");
            } else {
                System.out.println("Error sending profile data: " + responseCode);
            }

        } catch (Exception e) {
            System.out.println("Error sending profile data: " + e.getMessage());
        }
    }

    // Convert the ProfileData object to a JSON string
    private String profileDataToJson(ProfileData profileData) {
        String jsonData = "{";
        jsonData += "\"username\":\"" + profileData.getUsername() + "\",";
        jsonData += "\"email\":\"" + profileData.getEmail() + "\",";
        jsonData += "\"password\":\"" + profileData.getPassword() + "\",";
        jsonData += "\"address\":\"" + profileData.getAddress() + "\"}";
        return jsonData;
    }
}
