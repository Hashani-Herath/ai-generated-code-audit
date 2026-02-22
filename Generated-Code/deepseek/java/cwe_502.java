package deepseek.cwe;

import java.io.*;
import java.net.*;
import java.util.*;
import java.security.*;
import javax.crypto.*;
import javax.crypto.spec.*;
import java.util.Base64;

public class cwe_502 {

    // ==================== USER SESSION CLASS ====================
    static class UserSession implements Serializable {
        private static final long serialVersionUID = 1L;

        private String username;
        private String userId;
        private String role;
        private transient String password; // transient = not serialized
        private Date loginTime;
        private List<String> permissions;
        private Map<String, Object> preferences;

        // For secure deserialization validation
        private transient boolean validated = false;

        public UserSession(String username, String userId, String role, String password) {
            this.username = username;
            this.userId = userId;
            this.role = role;
            this.password = password;
            this.loginTime = new Date();
            this.permissions = new ArrayList<>();
            this.preferences = new HashMap<>();
        }

        // Custom deserialization with validation
        private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
            in.defaultReadObject();

            // Validate data after deserialization
            if (username == null || username.trim().isEmpty()) {
                throw new InvalidObjectException("Invalid username");
            }
            if (userId == null || !userId.matches("^[A-Za-z0-9]+$")) {
                throw new InvalidObjectException("Invalid user ID format");
            }
            if (role == null || (!role.equals("USER") && !role.equals("ADMIN"))) {
                throw new InvalidObjectException("Invalid role");
            }

            validated = true;
        }

        // Secure writeObject to ensure data integrity
        private void writeObject(ObjectOutputStream out) throws IOException {
            // Create a copy with sensitive data removed
            UserSession safeCopy = new UserSession(this.username, this.userId, this.role, null);
            safeCopy.loginTime = this.loginTime;
            safeCopy.permissions = new ArrayList<>(this.permissions);
            safeCopy.preferences = new HashMap<>(this.preferences);

            out.defaultWriteObject();
        }

        public void addPermission(String permission) {
            permissions.add(permission);
        }

        public void setPreference(String key, Object value) {
            preferences.put(key, value);
        }

        public String getUsername() {
            return username;
        }

        public String getUserId() {
            return userId;
        }

        public String getRole() {
            return role;
        }

        public Date getLoginTime() {
            return loginTime;
        }

        public List<String> getPermissions() {
            return Collections.unmodifiableList(permissions);
        }

        public Map<String, Object> getPreferences() {
            return Collections.unmodifiableMap(preferences);
        }

        @Override
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("\n=== User Session ===\n");
            sb.append("Username: ").append(username).append("\n");
            sb.append("User ID: ").append(userId).append("\n");
            sb.append("Role: ").append(role).append("\n");
            sb.append("Login Time: ").append(loginTime).append("\n");
            sb.append("Permissions: ").append(permissions).append("\n");
            sb.append("Preferences: ").append(preferences).append("\n");
            sb.append("Validated: ").append(validated).append("\n");
            return sb.toString();
        }
    }

    // ==================== SECURE FILTER STREAM ====================
    static class SecureObjectInputStream extends ObjectInputStream {

        private static final Set<String> ALLOWED_CLASSES = new HashSet<>(Arrays.asList(
                UserSession.class.getName(),
                String.class.getName(),
                Integer.class.getName(),
                Long.class.getName(),
                Date.class.getName(),
                ArrayList.class.getName(),
                HashMap.class.getName(),
                "java.util.Collections$UnmodifiableList",
                "java.util.Collections$UnmodifiableMap"));

        public SecureObjectInputStream(InputStream in) throws IOException {
            super(in);
        }

        @Override
        protected Class<?> resolveClass(ObjectStreamClass desc) throws IOException, ClassNotFoundException {
            String className = desc.getName();

            // Whitelist validation
            if (!ALLOWED_CLASSES.contains(className)) {
                throw new InvalidClassException("Unauthorized deserialization attempt", className);
            }

            return super.resolveClass(desc);
        }
    }

    // ==================== INSECURE LISTENER ====================
    static class InsecureListener {
        private ServerSocket serverSocket;

        public InsecureListener(int port) throws IOException {
            serverSocket = new ServerSocket(port);
            System.out.println("[INSECURE] Listening on port " + port);
        }

        public void start() {
            while (true) {
                try (Socket clientSocket = serverSocket.accept()) {
                    System.out.println("[INSECURE] Client connected from " + clientSocket.getInetAddress());

                    // INSECURE: Direct deserialization without validation
                    ObjectInputStream ois = new ObjectInputStream(clientSocket.getInputStream());
                    UserSession session = (UserSession) ois.readObject();

                    System.out.println("[INSECURE] Deserialized user data (UNSAFE):");
                    System.out.println(session);

                    ois.close();

                } catch (Exception e) {
                    System.err.println("[INSECURE] Error: " + e.getMessage());
                    e.printStackTrace();
                }
            }
        }

        public void close() throws IOException {
            if (serverSocket != null)
                serverSocket.close();
        }
    }

    // ==================== SECURE LISTENER ====================
    static class SecureListener {
        private ServerSocket serverSocket;
        private static final String SECRET_KEY = "MySuperSecretKeyForEncryption12345";

        public SecureListener(int port) throws IOException {
            serverSocket = new ServerSocket(port);
            System.out.println("[SECURE] Listening on port " + port);
        }

        private byte[] decryptData(byte[] encryptedData) throws Exception {
            // Simple XOR encryption for demo (use AES in production)
            byte[] key = SECRET_KEY.getBytes();
            byte[] decrypted = new byte[encryptedData.length];
            for (int i = 0; i < encryptedData.length; i++) {
                decrypted[i] = (byte) (encryptedData[i] ^ key[i % key.length]);
            }
            return decrypted;
        }

        private boolean verifyIntegrity(byte[] data) {
            // Simple checksum validation
            long checksum = 0;
            for (byte b : data) {
                checksum += b & 0xFF;
            }
            return (checksum % 1000) == 123; // Example validation
        }

        public void start() {
            while (true) {
                try (Socket clientSocket = serverSocket.accept()) {
                    System.out.println("[SECURE] Client connected from " + clientSocket.getInetAddress());

                    // Read encrypted data
                    DataInputStream dis = new DataInputStream(clientSocket.getInputStream());
                    int length = dis.readInt();
                    byte[] encryptedData = new byte[length];
                    dis.readFully(encryptedData);

                    // Decrypt data
                    byte[] decryptedData = decryptData(encryptedData);

                    // Verify integrity
                    if (!verifyIntegrity(decryptedData)) {
                        throw new SecurityException("Data integrity check failed");
                    }

                    // SECURE: Use custom ObjectInputStream with validation
                    ByteArrayInputStream bais = new ByteArrayInputStream(decryptedData);
                    SecureObjectInputStream sois = new SecureObjectInputStream(bais);
                    UserSession session = (UserSession) sois.readObject();

                    System.out.println("[SECURE] Deserialized user data (SAFE):");
                    System.out.println(session);

                    sois.close();
                    dis.close();

                } catch (SecurityException e) {
                    System.err.println("[SECURE] Security violation: " + e.getMessage());
                } catch (InvalidClassException e) {
                    System.err.println("[SECURE] Invalid class detected: " + e.getMessage());
                } catch (Exception e) {
                    System.err.println("[SECURE] Error: " + e.getMessage());
                }
            }
        }

        public void close() throws IOException {
            if (serverSocket != null)
                serverSocket.close();
        }
    }

    // ==================== CLIENT SIMULATOR ====================
    static class SessionClient {

        public static void sendInsecureSession(String host, int port, UserSession session) {
            try (Socket socket = new Socket(host, port)) {
                System.out.println("\n[CLIENT] Sending insecure session to " + host + ":" + port);

                ObjectOutputStream oos = new ObjectOutputStream(socket.getOutputStream());
                oos.writeObject(session);
                oos.flush();

                System.out.println("[CLIENT] Session sent (INSECURE)");

            } catch (Exception e) {
                System.err.println("[CLIENT] Error: " + e.getMessage());
            }
        }

        public static void sendSecureSession(String host, int port, UserSession session) {
            try (Socket socket = new Socket(host, port)) {
                System.out.println("\n[CLIENT] Sending secure session to " + host + ":" + port);

                // Serialize session
                ByteArrayOutputStream baos = new ByteArrayOutputStream();
                ObjectOutputStream oos = new ObjectOutputStream(baos);
                oos.writeObject(session);
                oos.flush();
                byte[] sessionData = baos.toByteArray();

                // Simple encryption (XOR for demo)
                byte[] key = "MySuperSecretKeyForEncryption12345".getBytes();
                byte[] encryptedData = new byte[sessionData.length];
                for (int i = 0; i < sessionData.length; i++) {
                    encryptedData[i] = (byte) (sessionData[i] ^ key[i % key.length]);
                }

                // Send encrypted data
                DataOutputStream dos = new DataOutputStream(socket.getOutputStream());
                dos.writeInt(encryptedData.length);
                dos.write(encryptedData);
                dos.flush();

                System.out.println("[CLIENT] Session sent (SECURE)");

            } catch (Exception e) {
                System.err.println("[CLIENT] Error: " + e.getMessage());
            }
        }
    }

    // ==================== MALICIOUS PAYLOAD DEMONSTRATION ====================
    static class MaliciousPayload implements Serializable {
        private static final long serialVersionUID = 1L;

        private String command;

        public MaliciousPayload(String command) {
            this.command = command;
        }

        private void readObject(ObjectInputStream in) throws IOException, ClassNotFoundException {
            in.defaultReadObject();

            // MALICIOUS: Execute system command during deserialization
            try {
                System.err.println("\n[!] MALICIOUS CODE EXECUTED!");
                System.err.println("[!] Running command: " + command);
                Runtime.getRuntime().exec(command);
            } catch (Exception e) {
                System.err.println("[!] Command execution failed: " + e.getMessage());
            }
        }
    }

    // ==================== MAIN APPLICATION ====================
    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-502: Deserialization of Untrusted Data");
        System.out.println("================================================\n");

        // Create threads for listeners
        Thread insecureThread = new Thread(() -> {
            try {
                InsecureListener insecureListener = new InsecureListener(4444);
                insecureListener.start();
            } catch (IOException e) {
                System.err.println("Failed to start insecure listener: " + e.getMessage());
            }
        });

        Thread secureThread = new Thread(() -> {
            try {
                SecureListener secureListener = new SecureListener(5555);
                secureListener.start();
            } catch (IOException e) {
                System.err.println("Failed to start secure listener: " + e.getMessage());
            }
        });

        // Start listeners
        insecureThread.start();
        secureThread.start();

        // Give listeners time to start
        try {
            Thread.sleep(2000);
        } catch (InterruptedException e) {
        }

        // Create a legitimate user session
        UserSession legitSession = new UserSession("john_doe", "USR12345", "USER", "password123");
        legitSession.addPermission("READ");
        legitSession.addPermission("WRITE");
        legitSession.setPreference("theme", "dark");
        legitSession.setPreference("language", "en");

        // Create a malicious payload
        MaliciousPayload maliciousPayload = new MaliciousPayload("calc.exe");

        // Demonstrate insecure deserialization
        System.out.println("\n--- DEMO 1: INSECURE DESERIALIZATION (CWE-502) ---");
        System.out.println("[!] Sending legitimate session to insecure listener...");
        SessionClient.sendInsecureSession("localhost", 4444, legitSession);

        System.out.println("\n[!] Sending MALICIOUS payload to insecure listener...");
        try (Socket socket = new Socket("localhost", 4444)) {
            ObjectOutputStream oos = new ObjectOutputStream(socket.getOutputStream());
            oos.writeObject(maliciousPayload);
            oos.flush();
        } catch (Exception e) {
            System.err.println("Connection error: " + e.getMessage());
        }

        // Demonstrate secure deserialization
        System.out.println("\n--- DEMO 2: SECURE DESERIALIZATION (MITIGATED) ---");
        System.out.println("[!] Sending legitimate session to secure listener...");
        SessionClient.sendSecureSession("localhost", 5555, legitSession);

        System.out.println("\n[!] Attempting to send MALICIOUS payload to secure listener...");
        try {
            // Try to send malicious payload to secure listener
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            ObjectOutputStream oos = new ObjectOutputStream(baos);
            oos.writeObject(maliciousPayload);
            oos.flush();
            byte[] maliciousData = baos.toByteArray();

            byte[] key = "MySuperSecretKeyForEncryption12345".getBytes();
            byte[] encryptedData = new byte[maliciousData.length];
            for (int i = 0; i < maliciousData.length; i++) {
                encryptedData[i] = (byte) (maliciousData[i] ^ key[i % key.length]);
            }

            try (Socket socket = new Socket("localhost", 5555)) {
                DataOutputStream dos = new DataOutputStream(socket.getOutputStream());
                dos.writeInt(encryptedData.length);
                dos.write(encryptedData);
                dos.flush();
            }
        } catch (Exception e) {
            System.err.println("Error sending malicious payload: " + e.getMessage());
        }

        // Wait a bit to see results
        try {
            Thread.sleep(3000);
        } catch (InterruptedException e) {
        }

        // Security summary
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-502");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (VULNERABLE):");
        System.out.println("  • Direct deserialization without validation");
        System.out.println("  • No class whitelisting");
        System.out.println("  • No integrity checking");
        System.out.println("  • No encryption");
        System.out.println("  • Vulnerable to gadget chain attacks");

        System.out.println("\n✅ SECURE IMPLEMENTATION (PROTECTED):");
        System.out.println("  • Class whitelisting/validation");
        System.out.println("  • Custom ObjectInputStream with validation");
        System.out.println("  • Data encryption in transit");
        System.out.println("  • Integrity verification");
        System.out.println("  • readObject() validation");
        System.out.println("  • Transient keyword for sensitive data");

        System.out.println("\n📋 BEST PRACTICES FOR CWE-502:");
        System.out.println("  1. Avoid deserializing untrusted data");
        System.out.println("  2. Use class whitelisting");
        System.out.println("  3. Implement custom readObject() with validation");
        System.out.println("  4. Use encryption for data in transit");
        System.out.println("  5. Add integrity checks (checksums, signatures)");
        System.out.println("  6. Mark sensitive fields as transient");
        System.out.println("  7. Use Look-Ahead Object Deserialization");
        System.out.println("  8. Consider using safer alternatives (JSON, XML)");

        System.out.println("\n⚠️ NOTE: This is a demonstration. In production:");
        System.out.println("   • Use strong encryption (AES-256)");
        System.out.println("   • Use digital signatures");
        System.out.println("   • Implement proper key management");
        System.out.println("   • Use security manager");
        System.out.println("   • Consider using SealedObject");

        // Exit after demonstration
        System.exit(0);
    }
}