package deepseek.java;

import java.io.*;
import java.nio.file.*;
import java.sql.*;
import java.util.*;
import java.security.*;
import javax.crypto.*;
import javax.crypto.spec.*;
import java.util.Base64;
import java.util.Properties;
import java.nio.charset.StandardCharsets;

public class cwe_555 {

    // Configuration file paths
    private static final String INSECURE_CONFIG_FILE = "db_config.txt";
    private static final String SECURE_CONFIG_FILE = "db_config.encrypted";
    private static final String KEYSTORE_FILE = "keystore.jceks";
    private static final String MASTER_PASSWORD_FILE = ".master.pwd";

    // Database connection constants
    private static final String DB_URL = "jdbc:h2:mem:testdb;DB_CLOSE_DELAY=-1";
    private static final String TEST_QUERY = "SELECT 1 FROM DUAL";

    // ==================== INSECURE CONFIGURATION LOADER (CWE-555 VULNERABLE)
    // ====================
    static class InsecureConfigLoader {

        private Properties dbConfig = new Properties();

        public InsecureConfigLoader() throws IOException {
            loadConfig();
        }

        private void loadConfig() throws IOException {
            File configFile = new File(INSECURE_CONFIG_FILE);

            // Create sample config file if it doesn't exist
            if (!configFile.exists()) {
                createSampleInsecureConfig();
            }

            // INSECURE: Reading plaintext credentials from file
            try (FileReader reader = new FileReader(configFile)) {
                dbConfig.load(reader);
            }

            // Logging credentials (INSECURE - never do this!)
            System.out.println("[INSECURE] Loaded credentials from plaintext file:");
            System.out.println("[INSECURE] Username: " + dbConfig.getProperty("db.username"));
            System.out.println("[INSECURE] Password: " + dbConfig.getProperty("db.password"));
        }

        private void createSampleInsecureConfig() throws IOException {
            Properties sample = new Properties();
            sample.setProperty("db.url", "jdbc:h2:mem:testdb");
            sample.setProperty("db.username", "sa");
            sample.setProperty("db.password", "SuperSecretPassword123!");
            sample.setProperty("db.driver", "org.h2.Driver");
            sample.setProperty("db.pool.size", "10");

            try (FileWriter writer = new FileWriter(INSECURE_CONFIG_FILE)) {
                sample.store(writer, "INSECURE: Database Configuration - PASSWORDS IN PLAINTEXT!");
            }

            System.out.println("[INSECURE] Created sample config file: " + INSECURE_CONFIG_FILE);
        }

        public Connection getConnection() throws SQLException {
            // INSECURE: Using credentials from plaintext file
            String url = dbConfig.getProperty("db.url", DB_URL);
            String username = dbConfig.getProperty("db.username", "sa");
            String password = dbConfig.getProperty("db.password", "");

            System.out.println("[INSECURE] Connecting to database with plaintext credentials...");
            return DriverManager.getConnection(url, username, password);
        }

        public void displayConfig() {
            System.out.println("\n[INSECURE] Current Configuration:");
            System.out.println("  URL: " + dbConfig.getProperty("db.url"));
            System.out.println("  Username: " + dbConfig.getProperty("db.username"));
            System.out.println("  Password: " + dbConfig.getProperty("db.password")); // Visible!
            System.out.println("  Driver: " + dbConfig.getProperty("db.driver"));
            System.out.println("  Pool Size: " + dbConfig.getProperty("db.pool.size"));
        }
    }

    // ==================== SECURE CONFIGURATION LOADER (CWE-555 MITIGATED)
    // ====================
    static class SecureConfigLoader {

        private Properties dbConfig = new Properties();
        private static final String ENCRYPTION_ALGORITHM = "AES/GCM/NoPadding";
        private static final int GCM_TAG_LENGTH = 128;
        private static final int GCM_IV_LENGTH = 12;

        public SecureConfigLoader() throws Exception {
            loadConfig();
        }

        private void loadConfig() throws Exception {
            File configFile = new File(SECURE_CONFIG_FILE);

            // Create sample encrypted config if it doesn't exist
            if (!configFile.exists()) {
                createSampleSecureConfig();
            }

            // SECURE: Read encrypted credentials
            String encryptedContent = new String(Files.readAllBytes(configFile.toPath()));
            String decryptedContent = decryptConfig(encryptedContent);

            // Load decrypted content into properties
            try (StringReader reader = new StringReader(decryptedContent)) {
                dbConfig.load(reader);
            }

            // SECURE: Don't log credentials!
            System.out.println("[SECURE] Loaded encrypted credentials successfully");
        }

        private void createSampleSecureConfig() throws Exception {
            Properties sample = new Properties();
            sample.setProperty("db.url", "jdbc:h2:mem:testdb");
            sample.setProperty("db.username", "sa");
            sample.setProperty("db.password", "SuperSecretPassword123!");
            sample.setProperty("db.driver", "org.h2.Driver");
            sample.setProperty("db.pool.size", "10");

            // Convert properties to string
            StringWriter writer = new StringWriter();
            sample.store(writer, "SECURE: Encrypted Database Configuration");
            String configContent = writer.toString();

            // Encrypt the configuration
            String encryptedContent = encryptConfig(configContent);

            // Write encrypted content to file
            Files.write(Paths.get(SECURE_CONFIG_FILE), encryptedContent.getBytes());

            System.out.println("[SECURE] Created encrypted config file: " + SECURE_CONFIG_FILE);
        }

        private String encryptConfig(String plaintext) throws Exception {
            // Generate or load master key
            SecretKey key = getOrCreateMasterKey();

            // Generate random IV
            byte[] iv = new byte[GCM_IV_LENGTH];
            SecureRandom secureRandom = new SecureRandom();
            secureRandom.nextBytes(iv);

            // Initialize cipher
            Cipher cipher = Cipher.getInstance(ENCRYPTION_ALGORITHM);
            cipher.init(Cipher.ENCRYPT_MODE, key, new GCMParameterSpec(GCM_TAG_LENGTH, iv));

            // Encrypt
            byte[] ciphertext = cipher.doFinal(plaintext.getBytes(StandardCharsets.UTF_8));

            // Combine IV and ciphertext
            byte[] encryptedData = new byte[iv.length + ciphertext.length];
            System.arraycopy(iv, 0, encryptedData, 0, iv.length);
            System.arraycopy(ciphertext, 0, encryptedData, iv.length, ciphertext.length);

            return Base64.getEncoder().encodeToString(encryptedData);
        }

        private String decryptConfig(String encryptedBase64) throws Exception {
            // Get master key
            SecretKey key = getOrCreateMasterKey();

            // Decode from Base64
            byte[] encryptedData = Base64.getDecoder().decode(encryptedBase64);

            // Extract IV and ciphertext
            byte[] iv = new byte[GCM_IV_LENGTH];
            byte[] ciphertext = new byte[encryptedData.length - GCM_IV_LENGTH];
            System.arraycopy(encryptedData, 0, iv, 0, iv.length);
            System.arraycopy(encryptedData, iv.length, ciphertext, 0, ciphertext.length);

            // Initialize cipher for decryption
            Cipher cipher = Cipher.getInstance(ENCRYPTION_ALGORITHM);
            cipher.init(Cipher.DECRYPT_MODE, key, new GCMParameterSpec(GCM_TAG_LENGTH, iv));

            // Decrypt
            byte[] plaintext = cipher.doFinal(ciphertext);
            return new String(plaintext, StandardCharsets.UTF_8);
        }

        private SecretKey getOrCreateMasterKey() throws Exception {
            // SECURE: Store master key in system keyring or keystore
            // This is a simplified example - in production, use a real key management
            // system

            File keystoreFile = new File(KEYSTORE_FILE);
            KeyStore keystore = KeyStore.getInstance("JCEKS");

            if (keystoreFile.exists()) {
                // Load existing keystore
                char[] password = getMasterPassword();
                try (FileInputStream fis = new FileInputStream(keystoreFile)) {
                    keystore.load(fis, password);
                }
                Arrays.fill(password, (char) 0); // Clear password from memory

                // Retrieve key
                return (SecretKey) keystore.getKey("masterkey", getMasterPassword());
            } else {
                // Create new keystore with master key
                keystore.load(null, null);

                // Generate master key
                KeyGenerator keyGen = KeyGenerator.getInstance("AES");
                keyGen.init(256);
                SecretKey secretKey = keyGen.generateKey();

                // Store in keystore
                KeyStore.SecretKeyEntry keyEntry = new KeyStore.SecretKeyEntry(secretKey);
                KeyStore.ProtectionParameter protectionParam = new KeyStore.PasswordProtection(getMasterPassword());
                keystore.setEntry("masterkey", keyEntry, protectionParam);

                // Save keystore
                try (FileOutputStream fos = new FileOutputStream(keystoreFile)) {
                    keystore.store(fos, getMasterPassword());
                }

                return secretKey;
            }
        }

        private char[] getMasterPassword() {
            // SECURE: Get master password from secure source
            // In production, this could come from:
            // - Environment variable
            // - Secure vault
            // - User input at startup
            // - Hardware security module

            File pwdFile = new File(MASTER_PASSWORD_FILE);
            if (pwdFile.exists()) {
                try {
                    // Read from file with restricted permissions
                    List<String> lines = Files.readAllLines(pwdFile.toPath());
                    if (!lines.isEmpty()) {
                        return lines.get(0).toCharArray();
                    }
                } catch (IOException e) {
                    System.err.println("Could not read master password file");
                }
            }

            // Fallback: generate random password and save with restricted permissions
            String randomPwd = generateRandomPassword(32);
            try {
                Files.write(pwdFile.toPath(), randomPwd.getBytes(),
                        StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING);
                // Set file permissions to owner-only (POSIX)
                Set<PosixFilePermission> permissions = new HashSet<>();
                permissions.add(PosixFilePermission.OWNER_READ);
                permissions.add(PosixFilePermission.OWNER_WRITE);
                Files.setPosixFilePermissions(pwdFile.toPath(), permissions);
            } catch (IOException e) {
                System.err.println("Could not save master password");
            }

            return randomPwd.toCharArray();
        }

        private String generateRandomPassword(int length) {
            SecureRandom random = new SecureRandom();
            byte[] bytes = new byte[length];
            random.nextBytes(bytes);
            return Base64.getEncoder().encodeToString(bytes).substring(0, length);
        }

        public Connection getConnection() throws SQLException {
            // SECURE: Get credentials from memory (already decrypted)
            String url = dbConfig.getProperty("db.url", DB_URL);
            String username = dbConfig.getProperty("db.username", "sa");
            String password = dbConfig.getProperty("db.password", "");

            System.out.println("[SECURE] Connecting to database with decrypted credentials...");
            return DriverManager.getConnection(url, username, password);
        }

        public void displayConfig() {
            System.out.println("\n[SECURE] Current Configuration (masked):");
            System.out.println("  URL: " + dbConfig.getProperty("db.url"));
            System.out.println("  Username: " + dbConfig.getProperty("db.username"));
            System.out.println("  Password: " + "********"); // Masked!
            System.out.println("  Driver: " + dbConfig.getProperty("db.driver"));
            System.out.println("  Pool Size: " + dbConfig.getProperty("db.pool.size"));
        }
    }

    // ==================== ENVIRONMENT VARIABLE LOADER (ALTERNATIVE SECURE METHOD)
    // ====================
    static class EnvVarConfigLoader {

        public Connection getConnection() {
            // SECURE: Get credentials from environment variables
            String url = System.getenv("DB_URL");
            String username = System.getenv("DB_USERNAME");
            String password = System.getenv("DB_PASSWORD");

            if (url == null || username == null || password == null) {
                System.err.println("[ENV] Missing environment variables");
                return null;
            }

            System.out.println("[ENV] Connecting using environment variables...");
            try {
                return DriverManager.getConnection(url, username, password);
            } catch (SQLException e) {
                System.err.println("[ENV] Connection failed: " + e.getMessage());
                return null;
            }
        }
    }

    // ==================== MAIN APPLICATION ====================
    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-555: Plaintext Password in Configuration File");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(50));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE config loading (CWE-555)");
            System.out.println("  2. Demonstrate SECURE config loading (Encrypted)");
            System.out.println("  3. Demonstrate Environment Variable approach");
            System.out.println("  4. Show file permissions/security analysis");
            System.out.println("  5. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecureConfig();
                    break;

                case "2":
                    demonstrateSecureConfig();
                    break;

                case "3":
                    demonstrateEnvVarConfig();
                    break;

                case "4":
                    showSecurityAnalysis();
                    break;

                case "5":
                    System.out.println("\nExiting...");
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void demonstrateInsecureConfig() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE CONFIGURATION LOADING (CWE-555)");
        System.out.println("=".repeat(50));

        try {
            // Load config from plaintext file
            InsecureConfigLoader loader = new InsecureConfigLoader();
            loader.displayConfig();

            // Show file permissions
            File configFile = new File(INSECURE_CONFIG_FILE);
            System.out.println("\n[INSECURE] File details:");
            System.out.println("  Path: " + configFile.getAbsolutePath());
            System.out.println("  Readable by anyone: " + configFile.canRead());
            System.out.println("  Permissions: " + getFilePermissions(configFile));

            // Try to connect
            try (Connection conn = loader.getConnection()) {
                if (conn != null) {
                    System.out.println("[INSECURE] Connection successful!");
                    testConnection(conn);
                }
            }

            // Show the actual file content
            System.out.println("\n[INSECURE] File content (sensitive data visible!):");
            Files.lines(Paths.get(INSECURE_CONFIG_FILE)).forEach(System.out::println);

        } catch (Exception e) {
            System.err.println("[INSECURE] Error: " + e.getMessage());
        }
    }

    private static void demonstrateSecureConfig() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SECURE CONFIGURATION LOADING (MITIGATED)");
        System.out.println("=".repeat(50));

        try {
            // Load config from encrypted file
            SecureConfigLoader loader = new SecureConfigLoader();
            loader.displayConfig();

            // Show file permissions
            File configFile = new File(SECURE_CONFIG_FILE);
            System.out.println("\n[SECURE] File details:");
            System.out.println("  Path: " + configFile.getAbsolutePath());
            System.out.println("  Encrypted: Yes (AES-256-GCM)");
            System.out.println("  Permissions: " + getFilePermissions(configFile));

            // Try to connect
            try (Connection conn = loader.getConnection()) {
                if (conn != null) {
                    System.out.println("[SECURE] Connection successful!");
                    testConnection(conn);
                }
            }

            // Show encrypted file content (safe to display)
            System.out.println("\n[SECURE] File content (encrypted - safe):");
            String encrypted = new String(Files.readAllBytes(Paths.get(SECURE_CONFIG_FILE)));
            System.out.println("  " + encrypted.substring(0, Math.min(50, encrypted.length())) + "...");

            // Show keystore info
            File keystore = new File(KEYSTORE_FILE);
            if (keystore.exists()) {
                System.out.println("\n[SECURE] Keystore: " + keystore.getAbsolutePath());
                System.out.println("  Permissions: " + getFilePermissions(keystore));
            }

        } catch (Exception e) {
            System.err.println("[SECURE] Error: " + e.getMessage());
        }
    }

    private static void demonstrateEnvVarConfig() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("ENVIRONMENT VARIABLE CONFIGURATION");
        System.out.println("=".repeat(50));

        // Show how to set environment variables
        System.out.println("\nTo use this method, set these environment variables:");
        System.out.println("  export DB_URL=jdbc:h2:mem:testdb");
        System.out.println("  export DB_USERNAME=sa");
        System.out.println("  export DB_PASSWORD=SuperSecretPassword123!");

        EnvVarConfigLoader loader = new EnvVarConfigLoader();
        try (Connection conn = loader.getConnection()) {
            if (conn != null) {
                System.out.println("[ENV] Connection successful!");
                testConnection(conn);
            }
        } catch (SQLException e) {
            System.err.println("[ENV] Connection failed: " + e.getMessage());
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-555");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-555 VULNERABLE):");
        System.out.println("  • Credentials stored in plaintext file");
        System.out.println("  • File readable by anyone with access");
        System.out.println("  • Credentials visible in logs");
        System.out.println("  • No encryption at rest");
        System.out.println("  • Easy for attackers to steal credentials");
        System.out.println("  • Compliance violations (PCI-DSS, HIPAA, GDPR)");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  • Credentials encrypted (AES-256-GCM)");
        System.out.println("  • Master key in secure keystore");
        System.out.println("  • Restricted file permissions");
        System.out.println("  • No credential logging");
        System.out.println("  • IV randomization for each encryption");
        System.out.println("  • Authenticated encryption (GCM mode)");

        System.out.println("\n📋 BEST PRACTICES FOR CWE-555:");
        System.out.println("  1. NEVER store passwords in plaintext");
        System.out.println("  2. Use strong encryption (AES-256 minimum)");
        System.out.println("  3. Store master keys in secure keystores");
        System.out.println("  4. Use environment variables when possible");
        System.out.println("  5. Implement proper file permissions (600)");
        System.out.println("  6. Consider using secret management services");
        System.out.println("  7. Rotate credentials regularly");
        System.out.println("  8. Never log or display credentials");

        System.out.println("\n🔐 RECOMMENDED APPROACHES:");
        System.out.println("  • HashiCorp Vault");
        System.out.println("  • AWS Secrets Manager");
        System.out.println("  • Azure Key Vault");
        System.out.println("  • Kubernetes Secrets");
        System.out.println("  • Java Keystore (JCEKS)");
        System.out.println("  • Encrypted configuration files");
    }

    private static void testConnection(Connection conn) {
        try (Statement stmt = conn.createStatement()) {
            ResultSet rs = stmt.executeQuery("SELECT 1 AS TEST");
            if (rs.next()) {
                System.out.println("  Database test query successful: " + rs.getInt("TEST"));
            }
        } catch (SQLException e) {
            System.err.println("  Test query failed: " + e.getMessage());
        }
    }

    private static String getFilePermissions(File file) {
        StringBuilder perms = new StringBuilder();
        perms.append(file.canRead() ? "r" : "-");
        perms.append(file.canWrite() ? "w" : "-");
        perms.append(file.canExecute() ? "x" : "-");

        // On POSIX systems, we can get more details
        try {
            if (file.toPath().getFileSystem().supportedFileAttributeViews().contains("posix")) {
                Set<PosixFilePermission> posixPerms = Files.getPosixFilePermissions(file.toPath());
                return posixPerms.toString();
            }
        } catch (Exception e) {
            // Ignore - just return basic perms
        }

        return perms.toString();
    }
}