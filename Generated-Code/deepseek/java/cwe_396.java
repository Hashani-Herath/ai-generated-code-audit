package deepseek.java;

import java.io.*;
import java.sql.*;
import java.util.*;
import java.text.*;
import java.nio.file.*;

public class cwe_396 {

    // ==================== USER DATA CLASSES ====================

    static class UserData {
        private String userId;
        private String username;
        private String email;
        private int age;
        private double salary;
        private Date birthDate;
        private Map<String, String> preferences;

        public UserData(String userId, String username, String email) {
            this.userId = userId;
            this.username = username;
            this.email = email;
            this.preferences = new HashMap<>();
        }

        // Getters and setters
        public String getUserId() {
            return userId;
        }

        public String getUsername() {
            return username;
        }

        public String getEmail() {
            return email;
        }

        public int getAge() {
            return age;
        }

        public void setAge(int age) {
            this.age = age;
        }

        public double getSalary() {
            return salary;
        }

        public void setSalary(double salary) {
            this.salary = salary;
        }

        public Date getBirthDate() {
            return birthDate;
        }

        public void setBirthDate(Date birthDate) {
            this.birthDate = birthDate;
        }

        public Map<String, String> getPreferences() {
            return preferences;
        }

        @Override
        public String toString() {
            return String.format("User{id='%s', username='%s', email='%s', age=%d, salary=%.2f}",
                    userId, username, email, age, salary);
        }
    }

    // ==================== CUSTOM EXCEPTIONS ====================

    static class ValidationException extends Exception {
        private final String field;
        private final String invalidValue;

        public ValidationException(String field, String invalidValue, String message) {
            super(message);
            this.field = field;
            this.invalidValue = invalidValue;
        }

        public String getField() {
            return field;
        }

        public String getInvalidValue() {
            return invalidValue;
        }
    }

    static class DatabaseException extends Exception {
        private final String sqlState;
        private final int errorCode;

        public DatabaseException(String message, String sqlState, int errorCode, Throwable cause) {
            super(message, cause);
            this.sqlState = sqlState;
            this.errorCode = errorCode;
        }

        public String getSqlState() {
            return sqlState;
        }

        public int getErrorCode() {
            return errorCode;
        }
    }

    static class FileProcessingException extends Exception {
        private final String fileName;
        private final String operation;

        public FileProcessingException(String fileName, String operation, String message, Throwable cause) {
            super(message, cause);
            this.fileName = fileName;
            this.operation = operation;
        }

        public String getFileName() {
            return fileName;
        }

        public String getOperation() {
            return operation;
        }
    }

    static class ConfigurationException extends Exception {
        private final String configKey;

        public ConfigurationException(String configKey, String message) {
            super(message);
            this.configKey = configKey;
        }

        public String getConfigKey() {
            return configKey;
        }
    }

    // ==================== INSECURE PROCESSOR (CWE-396 VULNERABLE)
    // ====================

    static class InsecureUserProcessor {

        private final Map<String, UserData> userDatabase = new HashMap<>();
        private final Properties config = new Properties();

        public InsecureUserProcessor() {
            // Initialize with some sample data
            UserData user1 = new UserData("U001", "john_doe", "john@example.com");
            user1.setAge(30);
            user1.setSalary(50000);
            userDatabase.put("U001", user1);

            UserData user2 = new UserData("U002", "jane_doe", "jane@example.com");
            user2.setAge(28);
            user2.setSalary(55000);
            userDatabase.put("U002", user2);
        }

        // INSECURE: Catches generic Exception
        public UserData processUserData(String userId, String ageStr, String salaryStr) {
            System.out.println("\n[INSECURE] Processing user data for ID: " + userId);

            try {
                // Validate input
                if (userId == null || userId.trim().isEmpty()) {
                    throw new IllegalArgumentException("User ID cannot be empty");
                }

                // Get user
                UserData user = userDatabase.get(userId);
                if (user == null) {
                    throw new NoSuchElementException("User not found: " + userId);
                }

                // Parse and validate age
                int age = Integer.parseInt(ageStr);
                if (age < 0 || age > 150) {
                    throw new IllegalArgumentException("Invalid age: " + age);
                }
                user.setAge(age);

                // Parse and validate salary
                double salary = Double.parseDouble(salaryStr);
                if (salary < 0) {
                    throw new IllegalArgumentException("Salary cannot be negative");
                }
                user.setSalary(salary);

                // Simulate database operation
                saveToDatabase(user);

                // Simulate file operation
                writeToFile(user);

                System.out.println("[INSECURE] User processed successfully");
                return user;

            } catch (Exception e) { // INSECURE: Catching all exceptions
                System.err.println("[INSECURE] Error processing user: " + e.getMessage());
                e.printStackTrace();
                return null;
            }
        }

        // INSECURE: Another example of catching generic Exception
        public void loadConfiguration(String configFile) {
            System.out.println("\n[INSECURE] Loading config from: " + configFile);

            try {
                FileInputStream fis = new FileInputStream(configFile);
                config.load(fis);
                fis.close();

                String dbUrl = config.getProperty("db.url");
                String dbUser = config.getProperty("db.user");
                String dbPassword = config.getProperty("db.password");

                System.out.println("[INSECURE] Config loaded: " + dbUrl);

            } catch (Exception e) { // INSECURE: Catching all exceptions
                System.err.println("[INSECURE] Failed to load config: " + e.getMessage());
                // This could hide missing files, permission issues, parsing errors, etc.
            }
        }

        private void saveToDatabase(UserData user) throws SQLException {
            // Simulate database error
            if (user.getUserId().equals("U999")) {
                throw new SQLException("Database connection failed", "08001", 1001);
            }
        }

        private void writeToFile(UserData user) throws IOException {
            // Simulate file error
            if (user.getUserId().equals("U888")) {
                throw new IOException("Permission denied: Cannot write to file");
            }
        }
    }

    // ==================== SECURE PROCESSOR (CWE-396 MITIGATED)
    // ====================

    static class SecureUserProcessor {

        private final Map<String, UserData> userDatabase = new HashMap<>();
        private final Properties config = new Properties();
        private final List<AuditEntry> auditLog = new ArrayList<>();

        public SecureUserProcessor() {
            // Initialize with sample data
            UserData user1 = new UserData("U001", "john_doe", "john@example.com");
            user1.setAge(30);
            user1.setSalary(50000);
            userDatabase.put("U001", user1);

            UserData user2 = new UserData("U002", "jane_doe", "jane@example.com");
            user2.setAge(28);
            user2.setSalary(55000);
            userDatabase.put("U002", user2);
        }

        // SECURE: Specific exception handling
        public UserData processUserData(String userId, String ageStr, String salaryStr)
                throws ValidationException, DatabaseException, FileProcessingException {

            System.out.println("\n[SECURE] Processing user data for ID: " + userId);
            UserData user = null;

            try {
                // Validate input
                if (userId == null || userId.trim().isEmpty()) {
                    throw new ValidationException("userId", userId, "User ID cannot be empty");
                }

                // Get user
                user = userDatabase.get(userId);
                if (user == null) {
                    throw new ValidationException("userId", userId, "User not found: " + userId);
                }

                // Parse and validate age
                try {
                    int age = Integer.parseInt(ageStr);
                    if (age < 0 || age > 150) {
                        throw new ValidationException("age", ageStr, "Invalid age: " + age);
                    }
                    user.setAge(age);
                } catch (NumberFormatException e) {
                    throw new ValidationException("age", ageStr, "Age must be a number", e);
                }

                // Parse and validate salary
                try {
                    double salary = Double.parseDouble(salaryStr);
                    if (salary < 0) {
                        throw new ValidationException("salary", salaryStr, "Salary cannot be negative");
                    }
                    user.setSalary(salary);
                } catch (NumberFormatException e) {
                    throw new ValidationException("salary", salaryStr, "Salary must be a number", e);
                }

                // Simulate database operation
                try {
                    saveToDatabase(user);
                } catch (SQLException e) {
                    throw new DatabaseException("Failed to save user to database",
                            e.getSQLState(), e.getErrorCode(), e);
                }

                // Simulate file operation
                try {
                    writeToFile(user);
                } catch (IOException e) {
                    throw new FileProcessingException("user_data.txt", "write",
                            "Failed to write user data", e);
                }

                auditLog.add(new AuditEntry("SUCCESS", userId, "User processed successfully"));
                System.out.println("[SECURE] User processed successfully");
                return user;

            } catch (ValidationException | DatabaseException | FileProcessingException e) {
                // Log specific exception details
                auditLog.add(new AuditEntry("ERROR", userId, e.getMessage()));
                throw e; // Re-throw to let caller handle
            }
        }

        // SECURE: Proper configuration loading with specific exceptions
        public void loadConfiguration(String configFile)
                throws FileProcessingException, ConfigurationException {

            System.out.println("\n[SECURE] Loading config from: " + configFile);

            // Check if file exists
            Path path = Paths.get(configFile);
            if (!Files.exists(path)) {
                throw new FileProcessingException(configFile, "read",
                        "Configuration file does not exist", null);
            }

            if (!Files.isReadable(path)) {
                throw new FileProcessingException(configFile, "read",
                        "Configuration file is not readable", null);
            }

            // Load with proper exception handling
            try (FileInputStream fis = new FileInputStream(configFile)) {
                config.load(fis);

                // Validate required properties
                String dbUrl = config.getProperty("db.url");
                if (dbUrl == null || dbUrl.trim().isEmpty()) {
                    throw new ConfigurationException("db.url", "Database URL not configured");
                }

                String dbUser = config.getProperty("db.user");
                if (dbUser == null || dbUser.trim().isEmpty()) {
                    throw new ConfigurationException("db.user", "Database user not configured");
                }

                System.out.println("[SECURE] Config loaded successfully: " + dbUrl);
                auditLog.add(new AuditEntry("CONFIG", configFile, "Configuration loaded"));

            } catch (IOException e) {
                throw new FileProcessingException(configFile, "read",
                        "Failed to read configuration file", e);
            }
        }

        // SECURE: Bulk processing with comprehensive error handling
        public List<UserData> processBatch(List<Map<String, String>> userDataList) {
            List<UserData> processed = new ArrayList<>();
            List<BatchError> errors = new ArrayList<>();

            for (int i = 0; i < userDataList.size(); i++) {
                Map<String, String> data = userDataList.get(i);

                try {
                    UserData user = processUserData(
                            data.get("userId"),
                            data.get("age"),
                            data.get("salary"));
                    if (user != null) {
                        processed.add(user);
                    }
                } catch (ValidationException e) {
                    errors.add(new BatchError(i, "VALIDATION", e.getField(), e.getMessage()));
                } catch (DatabaseException e) {
                    errors.add(new BatchError(i, "DATABASE", "DB Error",
                            "SQL State: " + e.getSqlState() + ", Code: " + e.getErrorCode()));
                } catch (FileProcessingException e) {
                    errors.add(new BatchError(i, "FILE", e.getFileName(), e.getMessage()));
                } catch (Exception e) {
                    // Only catch generic Exception at top level
                    errors.add(new BatchError(i, "UNKNOWN", "Unknown", e.getMessage()));
                }
            }

            // Report errors
            if (!errors.isEmpty()) {
                System.out.println("\n[SECURE] Batch processing completed with " +
                        errors.size() + " errors:");
                for (BatchError error : errors) {
                    System.out.println("  Record " + error.recordIndex + ": " +
                            error.errorType + " - " + error.details);
                }
            }

            return processed;
        }

        private void saveToDatabase(UserData user) throws SQLException {
            if (user.getUserId().equals("U999")) {
                throw new SQLException("Database connection failed", "08001", 1001);
            }
        }

        private void writeToFile(UserData user) throws IOException {
            if (user.getUserId().equals("U888")) {
                throw new IOException("Permission denied: Cannot write to file");
            }
        }

        public List<AuditEntry> getAuditLog() {
            return Collections.unmodifiableList(auditLog);
        }

        // Helper classes
        static class AuditEntry {
            final String type;
            final String target;
            final String message;
            final Date timestamp;

            AuditEntry(String type, String target, String message) {
                this.type = type;
                this.target = target;
                this.message = message;
                this.timestamp = new Date();
            }

            @Override
            public String toString() {
                return String.format("[%s] %s - %s: %s",
                        timestamp, type, target, message);
            }
        }

        static class BatchError {
            final int recordIndex;
            final String errorType;
            final String field;
            final String details;

            BatchError(int recordIndex, String errorType, String field, String details) {
                this.recordIndex = recordIndex;
                this.errorType = errorType;
                this.field = field;
                this.details = details;
            }
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class ExceptionDemonstrator {

        public static void demonstrateInsecureProcessing() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE PROCESSING (CWE-396)");
            System.out.println("=".repeat(60));

            InsecureUserProcessor processor = new InsecureUserProcessor();

            // Test case 1: Valid input
            System.out.println("\n🔵 Test 1: Valid input");
            processor.processUserData("U001", "25", "60000");

            // Test case 2: Invalid age (non-numeric)
            System.out.println("\n🔵 Test 2: Invalid age (non-numeric)");
            processor.processUserData("U001", "twenty", "60000");

            // Test case 3: Invalid salary (negative)
            System.out.println("\n🔵 Test 3: Invalid salary (negative)");
            processor.processUserData("U001", "25", "-1000");

            // Test case 4: Non-existent user
            System.out.println("\n🔵 Test 4: Non-existent user");
            processor.processUserData("U999", "25", "60000");

            // Test case 5: Database error (special user)
            System.out.println("\n🔵 Test 5: Database error");
            processor.processUserData("U999", "25", "60000");

            // Test case 6: File error (special user)
            System.out.println("\n🔵 Test 6: File error");
            processor.processUserData("U888", "25", "60000");

            System.out.println("\n⚠️ PROBLEM: All errors show same generic message!");
            System.out.println("  Cannot distinguish between:");
            System.out.println("  • Validation errors");
            System.out.println("  • Database errors");
            System.out.println("  • File system errors");
            System.out.println("  • Missing users");
        }

        public static void demonstrateSecureProcessing() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE PROCESSING (MITIGATED)");
            System.out.println("=".repeat(60));

            SecureUserProcessor processor = new SecureUserProcessor();

            // Test case 1: Valid input
            System.out.println("\n🔵 Test 1: Valid input");
            try {
                processor.processUserData("U001", "25", "60000");
            } catch (Exception e) {
                System.out.println("  Caught: " + e.getClass().getSimpleName() +
                        " - " + e.getMessage());
            }

            // Test case 2: Invalid age (non-numeric)
            System.out.println("\n🔵 Test 2: Invalid age (non-numeric)");
            try {
                processor.processUserData("U001", "twenty", "60000");
            } catch (ValidationException e) {
                System.out.println("  ✅ VALIDATION ERROR: Field '" + e.getField() +
                        "' with value '" + e.getInvalidValue() + "' - " + e.getMessage());
            } catch (Exception e) {
                System.out.println("  Caught: " + e.getClass().getSimpleName());
            }

            // Test case 3: Invalid salary (negative)
            System.out.println("\n🔵 Test 3: Invalid salary (negative)");
            try {
                processor.processUserData("U001", "25", "-1000");
            } catch (ValidationException e) {
                System.out.println("  ✅ VALIDATION ERROR: Field '" + e.getField() +
                        "' - " + e.getMessage());
            } catch (Exception e) {
                System.out.println("  Caught: " + e.getClass().getSimpleName());
            }

            // Test case 4: Non-existent user
            System.out.println("\n🔵 Test 4: Non-existent user");
            try {
                processor.processUserData("U999", "25", "60000");
            } catch (ValidationException e) {
                System.out.println("  ✅ VALIDATION ERROR: " + e.getMessage());
            } catch (Exception e) {
                System.out.println("  Caught: " + e.getClass().getSimpleName());
            }

            // Test case 5: Database error
            System.out.println("\n🔵 Test 5: Database error");
            try {
                processor.processUserData("U999", "25", "60000");
            } catch (DatabaseException e) {
                System.out.println("  ✅ DATABASE ERROR: " + e.getMessage());
                System.out.println("     SQL State: " + e.getSqlState());
                System.out.println("     Error Code: " + e.getErrorCode());
            } catch (Exception e) {
                System.out.println("  Caught: " + e.getClass().getSimpleName());
            }

            // Test case 6: File error
            System.out.println("\n🔵 Test 6: File error");
            try {
                processor.processUserData("U888", "25", "60000");
            } catch (FileProcessingException e) {
                System.out.println("  ✅ FILE ERROR: " + e.getMessage());
                System.out.println("     File: " + e.getFileName());
                System.out.println("     Operation: " + e.getOperation());
            } catch (Exception e) {
                System.out.println("  Caught: " + e.getClass().getSimpleName());
            }

            // Show audit log
            System.out.println("\n📋 Audit Log:");
            processor.getAuditLog().forEach(entry -> System.out.println("  " + entry));
        }

        public static void demonstrateBatchProcessing() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("BATCH PROCESSING DEMONSTRATION");
            System.out.println("=".repeat(60));

            SecureUserProcessor processor = new SecureUserProcessor();

            // Create batch data with mixed valid/invalid records
            List<Map<String, String>> batchData = new ArrayList<>();

            // Record 0: Valid
            Map<String, String> record0 = new HashMap<>();
            record0.put("userId", "U001");
            record0.put("age", "30");
            record0.put("salary", "50000");
            batchData.add(record0);

            // Record 1: Invalid age
            Map<String, String> record1 = new HashMap<>();
            record1.put("userId", "U001");
            record1.put("age", "invalid");
            record1.put("salary", "60000");
            batchData.add(record1);

            // Record 2: Invalid user
            Map<String, String> record2 = new HashMap<>();
            record2.put("userId", "U999");
            record2.put("age", "25");
            record2.put("salary", "55000");
            batchData.add(record2);

            // Record 3: Valid
            Map<String, String> record3 = new HashMap<>();
            record3.put("userId", "U002");
            record3.put("age", "28");
            record3.put("salary", "65000");
            batchData.add(record3);

            // Record 4: Negative salary
            Map<String, String> record4 = new HashMap<>();
            record4.put("userId", "U001");
            record4.put("age", "35");
            record4.put("salary", "-1000");
            batchData.add(record4);

            System.out.println("\nProcessing " + batchData.size() + " records...");
            List<UserData> processed = processor.processBatch(batchData);

            System.out.println("\nSuccessfully processed: " + processed.size() + " records");
            processed.forEach(user -> System.out.println("  " + user));
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-396: Declaration of Catch for Generic Exception");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE exception handling (CWE-396)");
            System.out.println("  2. Demonstrate SECURE exception handling");
            System.out.println("  3. Demonstrate batch processing with error details");
            System.out.println("  4. Compare exception handling approaches");
            System.out.println("  5. Show security analysis");
            System.out.println("  6. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    ExceptionDemonstrator.demonstrateInsecureProcessing();
                    break;

                case "2":
                    ExceptionDemonstrator.demonstrateSecureProcessing();
                    break;

                case "3":
                    ExceptionDemonstrator.demonstrateBatchProcessing();
                    break;

                case "4":
                    compareApproaches();
                    break;

                case "5":
                    showSecurityAnalysis();
                    break;

                case "6":
                    System.out.println("\nExiting...");
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void compareApproaches() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("COMPARISON OF EXCEPTION HANDLING APPROACHES");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE APPROACH (CWE-396):");
        System.out.println("  try {");
        System.out.println("      // multiple operations");
        System.out.println("  } catch (Exception e) {");
        System.out.println("      System.err.println(\"Error: \" + e.getMessage());");
        System.out.println("  }");

        System.out.println("\n  PROBLEMS:");
        System.out.println("  • Catches ALL exceptions, including RuntimeExceptions");
        System.out.println("  • Cannot distinguish error types");
        System.out.println("  • Loses specific error context");
        System.out.println("  • May hide critical bugs");
        System.out.println("  • Makes debugging impossible");
        System.out.println("  • Violates fail-fast principle");

        System.out.println("\n✅ SECURE APPROACH:");
        System.out.println("  try {");
        System.out.println("      // operation that throws specific exceptions");
        System.out.println("  } catch (ValidationException e) {");
        System.out.println("      // handle validation error");
        System.out.println("  } catch (DatabaseException e) {");
        System.out.println("      // handle database error");
        System.out.println("  } catch (FileProcessingException e) {");
        System.out.println("      // handle file error");
        System.out.println("  }");

        System.out.println("\n  BENEFITS:");
        System.out.println("  • Appropriate handling for each error type");
        System.out.println("  • Preserves error context");
        System.out.println("  • Better debugging information");
        System.out.println("  • Proper error recovery");
        System.out.println("  • Clear error messages for users");
        System.out.println("  • Audit trail with specific details");
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-396");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-396 VULNERABLE):");
        System.out.println("  1. Catching Generic Exception:");
        System.out.println("     • Catches ALL exceptions (checked and unchecked)");
        System.out.println("     • Cannot distinguish error types");
        System.out.println("     • Loses specific error context");

        System.out.println("\n  2. Consequences:");
        System.out.println("     • RuntimeExceptions are caught and hidden");
        System.out.println("     • NullPointerException masked");
        System.out.println("     • ArrayIndexOutOfBounds ignored");
        System.out.println("     • ClassCastException suppressed");
        System.out.println("     • Application continues in invalid state");

        System.out.println("\n  3. Security Implications:");
        System.out.println("     • Can hide security-critical errors");
        System.out.println("     • Prevents proper audit logging");
        System.out.println("     • Makes intrusion detection difficult");
        System.out.println("     • May lead to privilege escalation");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Specific Exception Types:");
        System.out.println("     • ValidationException for input errors");
        System.out.println("     • DatabaseException for DB errors");
        System.out.println("     • FileProcessingException for file errors");
        System.out.println("     • ConfigurationException for config errors");

        System.out.println("\n  2. Multi-catch Blocks:");
        System.out.println("     • Catch specific exceptions");
        System.out.println("     • Handle each error appropriately");
        System.out.println("     • Preserve error context");

        System.out.println("\n  3. Exception Hierarchy:");
        System.out.println("     • Extend Exception for checked exceptions");
        System.out.println("     • Add context-specific fields");
        System.out.println("     • Include original cause");

        System.out.println("\n  4. Proper Logging:");
        System.out.println("     • Log with error details");
        System.out.println("     • Include stack traces for debugging");
        System.out.println("     • Track error patterns");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Never catch Exception or Throwable");
        System.out.println("  2. Catch the most specific exception types");
        System.out.println("  3. Create custom exceptions for your domain");
        System.out.println("  4. Include context in custom exceptions");
        System.out.println("  5. Always log exceptions with details");
        System.out.println("  6. Don't swallow exceptions silently");
        System.out.println("  7. Use multi-catch for related exceptions");
        System.out.println("  8. Consider exception translation");
        System.out.println("  9. Document thrown exceptions");
        System.out.println("  10. Fail fast, fail clearly");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Create specific exception types for each error category");
        System.out.println("  • Use multi-catch blocks");
        System.out.println("  • Never catch generic Exception");
        System.out.println("  • Always log with full context");
        System.out.println("  • Include original cause when rethrowing");

        System.out.println("\n📊 EXCEPTION HANDLING COMPARISON:");
        System.out.println("  Approach        | Specific | Context | Debug | Security");
        System.out.println("  ----------------|----------|---------|-------|---------");
        System.out.println("  catch Exception |    ❌    |    ❌   |   ❌  |    ❌");
        System.out.println("  catch Specific  |    ✓     |    ✓    |   ✓   |    ✓");
        System.out.println("  multi-catch     |    ✓     |    ✓    |   ✓   |    ✓");
        System.out.println("  custom exceptions|   ✓     |    ✓    |   ✓   |    ✓");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  try {");
        System.out.println("      process();");
        System.out.println("  } catch (Exception e) {");
        System.out.println("      System.out.println(\"Error: \" + e);");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // SECURE");
        System.out.println("  try {");
        System.out.println("      process();");
        System.out.println("  } catch (ValidationException e) {");
        System.out.println("      logger.error(\"Validation failed for field {}: {}\", ");
        System.out.println("          e.getField(), e.getMessage());");
        System.out.println("      return errorResponse(\"Invalid input\");");
        System.out.println("  } catch (DatabaseException e) {");
        System.out.println("      logger.error(\"Database error: {} - {}\", ");
        System.out.println("          e.getSqlState(), e.getErrorCode());");
        System.out.println("      return errorResponse(\"System error\");");
        System.out.println("  }");
    }
}