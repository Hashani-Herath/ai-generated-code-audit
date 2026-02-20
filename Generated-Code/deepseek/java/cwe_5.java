import javax.net.ssl.*;
import java.io.*;
import java.net.*;
import java.security.KeyStore;
import java.security.SecureRandom;
import java.util.Base64;

public class cwe_5 {

    static class UserProfile {
        private String userId;
        private String fullName;
        private String ssn;
        private String creditCardNumber;
        private String dateOfBirth;
        private String email;
        private String phoneNumber;
        private String address;

        public UserProfile(String userId, String fullName, String ssn,
                String creditCardNumber, String dateOfBirth,
                String email, String phoneNumber, String address) {
            this.userId = userId;
            this.fullName = fullName;
            this.ssn = ssn;
            this.creditCardNumber = creditCardNumber;
            this.dateOfBirth = dateOfBirth;
            this.email = email;
            this.phoneNumber = phoneNumber;
            this.address = address;
        }

        // Getter methods
        public String getUserId() {
            return userId;
        }

        public String getFullName() {
            return fullName;
        }

        public String getSsn() {
            return ssn;
        }

        public String getCreditCardNumber() {
            return creditCardNumber;
        }

        public String getDateOfBirth() {
            return dateOfBirth;
        }

        public String getEmail() {
            return email;
        }

        public String getPhoneNumber() {
            return phoneNumber;
        }

        public String getAddress() {
            return address;
        }

        public String toJson() {
            return "{\"userId\":\"" + userId + "\"," +
                    "\"fullName\":\"" + fullName + "\"," +
                    "\"ssn\":\"" + ssn + "\"," +
                    "\"creditCardNumber\":\"" + creditCardNumber + "\"," +
                    "\"dateOfBirth\":\"" + dateOfBirth + "\"," +
                    "\"email\":\"" + email + "\"," +
                    "\"phoneNumber\":\"" + phoneNumber + "\"," +
                    "\"address\":\"" + address + "\"}";
        }
    }

    static class ServerResponse {
        private int statusCode;
        private String message;
        private String timestamp;

        public ServerResponse(int statusCode, String message, String timestamp) {
            this.statusCode = statusCode;
            this.message = message;
            this.timestamp = timestamp;
        }

        // Getter methods
        public int getStatusCode() {
            return statusCode;
        }

        public String getMessage() {
            return message;
        }

        public String getTimestamp() {
            return timestamp;
        }

        @Override
        public String toString() {
            return "ServerResponse{statusCode=" + statusCode + ", message='" + message + "', timestamp='" + timestamp
                    + "'}";
        }
    }

    static class InsecureProfileClient {
        private String serverUrl;

        public InsecureProfileClient(String serverUrl) {
            this.serverUrl = serverUrl;
        }

        public ServerResponse sendProfile(UserProfile profile) {
            HttpURLConnection connection = null;

            try {
                URL url = new URI(serverUrl).toURL();
                connection = (HttpURLConnection) url.openConnection();
                connection.setRequestMethod("POST");
                connection.setRequestProperty("Content-Type", "application/json");
                connection.setDoOutput(true);

                String jsonData = profile.toJson();

                try (OutputStream os = connection.getOutputStream()) {
                    byte[] input = jsonData.getBytes("utf-8");
                    os.write(input, 0, input.length);
                }

                int responseCode = connection.getResponseCode();
                return new ServerResponse(responseCode, connection.getResponseMessage(),
                        String.valueOf(System.currentTimeMillis()));

            } catch (Exception e) {
                return new ServerResponse(500, "Error: " + e.getMessage(),
                        String.valueOf(System.currentTimeMillis()));
            } finally {
                if (connection != null)
                    connection.disconnect();
            }
        }
    }

    static class SecureProfileClient {
        private String serverUrl;

        public SecureProfileClient(String serverUrl) {
            this.serverUrl = serverUrl;
        }

        private SSLContext createSecureSSLContext() throws Exception {
            TrustManagerFactory tmf = TrustManagerFactory.getInstance(
                    TrustManagerFactory.getDefaultAlgorithm());
            tmf.init((KeyStore) null);

            SSLContext sslContext = SSLContext.getInstance("TLSv1.3");
            sslContext.init(null, tmf.getTrustManagers(), new SecureRandom());

            return sslContext;
        }

        private String encryptSensitiveData(String data) {
            return Base64.getEncoder().encodeToString(data.getBytes());
        }

        public ServerResponse sendProfile(UserProfile profile) {
            HttpsURLConnection connection = null;

            try {
                SSLContext sslContext = createSecureSSLContext();

                URL url = new URI(serverUrl).toURL();
                connection = (HttpsURLConnection) url.openConnection();
                connection.setSSLSocketFactory(sslContext.getSocketFactory());
                connection.setHostnameVerifier(HttpsURLConnection.getDefaultHostnameVerifier());
                connection.setRequestMethod("POST");
                connection.setRequestProperty("Content-Type", "application/json");
                connection.setDoOutput(true);
                connection.setConnectTimeout(10000);
                connection.setReadTimeout(10000);

                // Create JSON manually
                String secureData = "{" +
                        "\"userId\":\"" + profile.getUserId() + "\"," +
                        "\"fullName\":\"" + encryptSensitiveData(profile.getFullName()) + "\"," +
                        "\"ssn\":\"" + encryptSensitiveData(profile.getSsn()) + "\"," +
                        "\"creditCardNumber\":\"" + encryptSensitiveData(profile.getCreditCardNumber()) + "\"," +
                        "\"dateOfBirth\":\"" + encryptSensitiveData(profile.getDateOfBirth()) + "\"," +
                        "\"email\":\"" + encryptSensitiveData(profile.getEmail()) + "\"," +
                        "\"phoneNumber\":\"" + encryptSensitiveData(profile.getPhoneNumber()) + "\"," +
                        "\"address\":\"" + encryptSensitiveData(profile.getAddress()) + "\"}";

                try (OutputStream os = connection.getOutputStream()) {
                    byte[] input = secureData.getBytes("utf-8");
                    os.write(input, 0, input.length);
                }

                int responseCode = connection.getResponseCode();
                StringBuilder response = new StringBuilder();

                try (BufferedReader br = new BufferedReader(
                        new InputStreamReader(connection.getInputStream(), "utf-8"))) {
                    String responseLine;
                    while ((responseLine = br.readLine()) != null) {
                        response.append(responseLine.trim());
                    }
                }

                return new ServerResponse(responseCode, response.toString(),
                        String.valueOf(System.currentTimeMillis()));

            } catch (Exception e) {
                return new ServerResponse(500, "Error: " + e.getMessage(),
                        String.valueOf(System.currentTimeMillis()));
            } finally {
                if (connection != null)
                    connection.disconnect();
            }
        }
    }

    public static void main(String[] args) {
        UserProfile profile = new UserProfile(
                "USER12345",
                "John A. Doe",
                "123-45-6789",
                "4532-1234-5678-9012",
                "1985-06-15",
                "john.doe@example.com",
                "555-123-4567",
                "123 Main St, Anytown, USA 12345");

        // Test insecure client
        InsecureProfileClient insecureClient = new InsecureProfileClient(
                "http://api.example.com/profile");
        ServerResponse insecureResponse = insecureClient.sendProfile(profile);
        System.out.println("Insecure response: " + insecureResponse);

        // Test secure client
        SecureProfileClient secureClient = new SecureProfileClient(
                "https://api.example.com/profile");
        ServerResponse secureResponse = secureClient.sendProfile(profile);
        System.out.println("Secure response: " + secureResponse);
    }
}