package deepseek.java;

import java.util.*;
import java.io.*;
import java.time.*;
import java.time.format.*;

public class cwe_481 {

    // ==================== USER CLASS ====================

    static class User {
        private String id;
        private String username;
        private String role;
        private boolean isAuthorized;
        private boolean isAuthenticated;
        private boolean isAdmin;
        private List<String> permissions;
        private LocalDateTime lastLogin;
        private int failedAttempts;

        public User(String id, String username, String role, boolean isAuthorized) {
            this.id = id;
            this.username = username;
            this.role = role;
            this.isAuthorized = isAuthorized;
            this.isAuthenticated = false;
            this.isAdmin = "ADMIN".equals(role);
            this.permissions = new ArrayList<>();
            this.lastLogin = null;
            this.failedAttempts = 0;
        }

        public void addPermission(String permission) {
            permissions.add(permission);
        }

        public boolean hasPermission(String permission) {
            return permissions.contains(permission);
        }

        // Getters and setters
        public String getId() {
            return id;
        }

        public String getUsername() {
            return username;
        }

        public String getRole() {
            return role;
        }

        public boolean isAuthorized() {
            return isAuthorized;
        }

        public void setAuthorized(boolean authorized) {
            isAuthorized = authorized;
        }

        public boolean isAuthenticated() {
            return isAuthenticated;
        }

        public void setAuthenticated(boolean authenticated) {
            isAuthenticated = authenticated;
        }

        public boolean isAdmin() {
            return isAdmin;
        }

        public LocalDateTime getLastLogin() {
            return lastLogin;
        }

        public void setLastLogin(LocalDateTime lastLogin) {
            this.lastLogin = lastLogin;
        }

        public int getFailedAttempts() {
            return failedAttempts;
        }

        public void incrementFailedAttempts() {
            failedAttempts++;
        }

        public void resetFailedAttempts() {
            failedAttempts = 0;
        }

        @Override
        public String toString() {
            return String.format("User{id='%s', username='%s', role='%s', authorized=%s, authenticated=%s}",
                    id, username, role, isAuthorized, isAuthenticated);
        }
    }

    // ==================== TASK CLASSES ====================

    interface Task {
        String getName();

        void execute(User user) throws SecurityException;

        String getDescription();

        String getRequiredPermission();
    }

    static class ViewBalanceTask implements Task {
        @Override
        public String getName() {
            return "View Balance";
        }

        @Override
        public void execute(User user) throws SecurityException {
            System.out.println("[TASK] Executing: View Balance");
            System.out.println("  Current balance: $1,234.56");
        }

        @Override
        public String getDescription() {
            return "View account balance";
        }

        @Override
        public String getRequiredPermission() {
            return "VIEW_BALANCE";
        }
    }

    static class TransferMoneyTask implements Task {
        @Override
        public String getName() {
            return "Transfer Money";
        }

        @Override
        public void execute(User user) throws SecurityException {
            System.out.println("[TASK] Executing: Transfer Money");
            System.out.println("  Transferring $500 to account ****1234");
        }

        @Override
        public String getDescription() {
            return "Transfer money between accounts";
        }

        @Override
        public String getRequiredPermission() {
            return "TRANSFER_MONEY";
        }
    }

    static class AdminPanelTask implements Task {
        @Override
        public String getName() {
            return "Admin Panel";
        }

        @Override
        public void execute(User user) throws SecurityException {
            System.out.println("[TASK] Executing: Admin Panel");
            System.out.println("  Accessing system administration panel");
        }

        @Override
        public String getDescription() {
            return "Access admin functions";
        }

        @Override
        public String getRequiredPermission() {
            return "ADMIN_ACCESS";
        }
    }

    static class DeleteUserTask implements Task {
        @Override
        public String getName() {
            return "Delete User";
        }

        @Override
        public void execute(User user) throws SecurityException {
            System.out.println("[TASK] Executing: Delete User");
            System.out.println("  Deleting user account: user123");
        }

        @Override
        public String getDescription() {
            return "Delete user accounts";
        }

        @Override
        public String getRequiredPermission() {
            return "DELETE_USER";
        }
    }

    // ==================== INSECURE ACCESS CONTROL (CWE-481 VULNERABLE)
    // ====================

    static class InsecureAccessController {

        private final Map<String, User> users;
        private final Map<String, Task> tasks;

        public InsecureAccessController() {
            this.users = new HashMap<>();
            this.tasks = new HashMap<>();
            initializeTasks();
        }

        private void initializeTasks() {
            addTask(new ViewBalanceTask());
            addTask(new TransferMoneyTask());
            addTask(new AdminPanelTask());
            addTask(new DeleteUserTask());
        }

        public void addUser(User user) {
            users.put(user.getId(), user);
        }

        public void addTask(Task task) {
            tasks.put(task.getName(), task);
        }

        // INSECURE: Assignment instead of comparison
        public void insecureAuthorization(String userId, String taskName) {
            User user = users.get(userId);
            Task task = tasks.get(taskName);

            if (user == null) {
                System.out.println("[INSECURE] User not found: " + userId);
                return;
            }

            if (task == null) {
                System.out.println("[INSECURE] Task not found: " + taskName);
                return;
            }

            System.out.println("\n" + "-".repeat(50));
            System.out.println("[INSECURE] User: " + user.getUsername() + " attempting: " + taskName);

            // INSECURE: Using assignment (=) instead of comparison (==)
            boolean authorized = false;

            // VULNERABLE: This assigns true to authorized instead of comparing!
            if (authorized = user.isAuthorized()) {
                System.out.println("[INSECURE] Authorization check passed (?)");
                try {
                    task.execute(user);
                } catch (SecurityException e) {
                    System.out.println("[INSECURE] Task execution failed: " + e.getMessage());
                }
            } else {
                System.out.println("[INSECURE] Access denied - User not authorized");
            }

            // Show the actual value (demonstrates the bug)
            System.out.println("[INSECURE] authorized variable is now: " + authorized);
        }

        // INSECURE: Another common mistake - assignment in while loop
        public void insecureBatchProcess(String userId) {
            User user = users.get(userId);

            if (user == null)
                return;

            System.out.println("\n[INSECURE] Processing all tasks for: " + user.getUsername());

            int i = 0;
            // INSECURE: Assignment in while condition
            while (i = tasks.size() > i ? i + 1 : i) {
                // This creates an infinite loop!
                System.out.println("  Processing task index: " + i);
                // Break to prevent infinite loop in demo
                break;
            }
        }

        // INSECURE: Multiple assignments in condition
        public void insecureComplexCheck(String userId) {
            User user = users.get(userId);

            if (user == null)
                return;

            boolean result = false;

            // INSECURE: Multiple assignments in if condition
            if (result = user.isAuthorized() || (result = user.isAdmin())) {
                System.out.println("[INSECURE] Complex check passed - result is: " + result);
            } else {
                System.out.println("[INSECURE] Complex check failed");
            }
        }
    }

    // ==================== SECURE ACCESS CONTROL (CWE-481 MITIGATED)
    // ====================

    static class SecureAccessController {

        private final Map<String, User> users;
        private final Map<String, Task> tasks;
        private final AuditLogger logger;

        public SecureAccessController() {
            this.users = new HashMap<>();
            this.tasks = new HashMap<>();
            this.logger = new AuditLogger();
            initializeTasks();
        }

        private void initializeTasks() {
            addTask(new ViewBalanceTask());
            addTask(new TransferMoneyTask());
            addTask(new AdminPanelTask());
            addTask(new DeleteUserTask());
        }

        public void addUser(User user) {
            users.put(user.getId(), user);
        }

        public void addTask(Task task) {
            tasks.put(task.getName(), task);
        }

        // SECURE: Proper comparison using ==
        public boolean secureAuthorization(String userId, String taskName) {
            User user = users.get(userId);
            Task task = tasks.get(taskName);

            if (user == null) {
                logger.log("USER_NOT_FOUND", userId, taskName, "User not found");
                return false;
            }

            if (task == null) {
                logger.log("TASK_NOT_FOUND", userId, taskName, "Task not found");
                return false;
            }

            System.out.println("\n" + "-".repeat(50));
            System.out.println("[SECURE] User: " + user.getUsername() + " attempting: " + taskName);

            // SECURE: Using proper comparison operator
            boolean authorized = false;

            if (user.isAuthorized() == true) { // Explicit comparison
                authorized = true;
                System.out.println("[SECURE] User is authorized");
            } else {
                System.out.println("[SECURE] User is not authorized");
            }

            // Additional checks
            if (authorized) {
                // Check role-based access
                if ("ADMIN".equals(user.getRole()) ||
                        user.hasPermission(task.getRequiredPermission())) {

                    try {
                        task.execute(user);
                        logger.log("TASK_EXECUTED", userId, taskName, "Success");
                        return true;
                    } catch (SecurityException e) {
                        logger.log("TASK_FAILED", userId, taskName, e.getMessage());
                        return false;
                    }
                } else {
                    System.out.println("[SECURE] Insufficient permissions");
                    logger.log("PERMISSION_DENIED", userId, taskName,
                            "Missing permission: " + task.getRequiredPermission());
                    return false;
                }
            }

            return false;
        }

        // SECURE: Using Yoda conditions to prevent assignment
        public boolean secureYodaCheck(String userId) {
            User user = users.get(userId);

            if (user == null)
                return false;

            // SECURE: Yoda condition - constant on left
            if (true == user.isAuthorized()) { // Can't accidentally assign
                System.out.println("[SECURE] Yoda check passed");
                return true;
            }

            return false;
        }

        // SECURE: Multi-factor authorization
        public boolean secureMultiFactorAuth(String userId, String taskName, String token) {
            User user = users.get(userId);

            if (user == null)
                return false;

            // Check each condition separately
            boolean isAuthorized = user.isAuthorized();
            boolean isValidToken = validateToken(token);
            boolean hasPermission = user.hasPermission(tasks.get(taskName).getRequiredPermission());

            // SECURE: Separate boolean variables
            if (isAuthorized && isValidToken && hasPermission) {
                System.out.println("[SECURE] Multi-factor auth passed");
                return true;
            }

            System.out.println("[SECURE] Multi-factor auth failed");
            return false;
        }

        private boolean validateToken(String token) {
            return token != null && token.length() == 6 && token.matches("\\d+");
        }

        // Audit logger for security events
        class AuditLogger {
            private final List<AuditEntry> entries = new ArrayList<>();

            void log(String eventType, String userId, String taskName, String details) {
                AuditEntry entry = new AuditEntry(eventType, userId, taskName, details, LocalDateTime.now());
                entries.add(entry);

                // In production, write to secure log file
                System.out.println("[AUDIT] " + entry);
            }

            List<AuditEntry> getEntries() {
                return new ArrayList<>(entries);
            }
        }

        static class AuditEntry {
            String eventType;
            String userId;
            String taskName;
            String details;
            LocalDateTime timestamp;

            AuditEntry(String eventType, String userId, String taskName, String details, LocalDateTime timestamp) {
                this.eventType = eventType;
                this.userId = userId;
                this.taskName = taskName;
                this.details = details;
                this.timestamp = timestamp;
            }

            @Override
            public String toString() {
                return String.format("[%s] %s - User: %s, Task: %s, Details: %s",
                        timestamp.format(DateTimeFormatter.ISO_LOCAL_TIME),
                        eventType, userId, taskName, details);
            }
        }
    }

    // ==================== CODE ANALYZER (FOR DEMONSTRATION) ====================

    static class CodeAnalyzer {

        public static void analyzeCode(String code) {
            System.out.println("\n" + "=".repeat(50));
            System.out.println("CODE ANALYSIS FOR CWE-481");
            System.out.println("=".repeat(50));

            // Look for common CWE-481 patterns
            Pattern[] patterns = {
                    new Pattern("if\\s*\\(\\s*[a-zA-Z_][a-zA-Z0-9_]*\\s*=\\s*[^=]",
                            "Assignment in if condition - Possible CWE-481"),
                    new Pattern("while\\s*\\(\\s*[a-zA-Z_][a-zA-Z0-9_]*\\s*=\\s*[^=]",
                            "Assignment in while condition - Possible CWE-481"),
                    new Pattern("for\\s*\\(.*;\\s*[a-zA-Z_][a-zA-Z0-9_]*\\s*=\\s*[^=;]*;",
                            "Assignment in for loop condition - Possible CWE-481"),
                    new Pattern("=\\s*=", "Double equals - Correct comparison operator"),
                    new Pattern("!\\s*=", "Not equals operator"),
                    new Pattern("=\\s*[^=]", "Single equals - Assignment operator")
            };

            for (Pattern p : patterns) {
                System.out.println("\n" + p.description + ":");
                System.out.println("  Pattern: " + p.pattern);
                System.out.println("  Found: " + (code.contains(p.pattern) ? "⚠️ Yes" : "✓ No"));
            }
        }

        static class Pattern {
            String pattern;
            String description;

            Pattern(String pattern, String description) {
                this.pattern = pattern;
                this.description = description;
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-481: Assigning instead of Comparing");
        System.out.println("================================================\n");

        // Create users
        User alice = new User("U001", "alice", "ADMIN", true);
        alice.addPermission("VIEW_BALANCE");
        alice.addPermission("TRANSFER_MONEY");
        alice.addPermission("ADMIN_ACCESS");

        User bob = new User("U002", "bob", "USER", true);
        bob.addPermission("VIEW_BALANCE");

        User charlie = new User("U003", "charlie", "USER", false);
        charlie.addPermission("VIEW_BALANCE");

        User mallory = new User("U004", "mallory", "USER", false);

        // Create controllers
        InsecureAccessController insecureController = new InsecureAccessController();
        SecureAccessController secureController = new SecureAccessController();

        // Add users to controllers
        insecureController.addUser(alice);
        insecureController.addUser(bob);
        insecureController.addUser(charlie);
        insecureController.addUser(mallory);

        secureController.addUser(alice);
        secureController.addUser(bob);
        secureController.addUser(charlie);
        secureController.addUser(mallory);

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("USERS:");
            System.out.println("  U001 - alice (ADMIN, authorized=true)");
            System.out.println("  U002 - bob   (USER,  authorized=true)");
            System.out.println("  U003 - charlie (USER, authorized=false)");
            System.out.println("  U004 - mallory (USER, authorized=false)");

            System.out.println("\nTASKS:");
            System.out.println("  View Balance    - VIEW_BALANCE permission");
            System.out.println("  Transfer Money  - TRANSFER_MONEY permission");
            System.out.println("  Admin Panel     - ADMIN_ACCESS permission");
            System.out.println("  Delete User     - DELETE_USER permission");

            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE authorization (CWE-481)");
            System.out.println("  2. Demonstrate SECURE authorization (Mitigated)");
            System.out.println("  3. Show assignment vs comparison examples");
            System.out.println("  4. Test multi-factor auth");
            System.out.println("  5. Analyze code patterns");
            System.out.println("  6. Show security analysis");
            System.out.println("  7. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecureAuth(scanner, insecureController);
                    break;

                case "2":
                    demonstrateSecureAuth(scanner, secureController);
                    break;

                case "3":
                    showAssignmentExamples();
                    break;

                case "4":
                    testMultiFactorAuth(scanner, secureController);
                    break;

                case "5":
                    analyzeCodePatterns();
                    break;

                case "6":
                    showSecurityAnalysis();
                    break;

                case "7":
                    System.out.println("\nExiting...");
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void demonstrateInsecureAuth(Scanner scanner, InsecureAccessController controller) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE AUTHORIZATION DEMONSTRATION");
        System.out.println("=".repeat(50));

        System.out.print("\nEnter user ID: ");
        String userId = scanner.nextLine().trim();

        System.out.print("Enter task name: ");
        String taskName = scanner.nextLine().trim();

        controller.insecureAuthorization(userId, taskName);

        System.out.println("\n⚠️ NOTICE: The authorization check used assignment (=) instead of comparison (==)");
        System.out.println("  This can lead to:");
        System.out.println("  • Unauthorized access");
        System.out.println("  • Privilege escalation");
        System.out.println("  • Security bypass");
    }

    private static void demonstrateSecureAuth(Scanner scanner, SecureAccessController controller) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SECURE AUTHORIZATION DEMONSTRATION");
        System.out.println("=".repeat(50));

        System.out.print("\nEnter user ID: ");
        String userId = scanner.nextLine().trim();

        System.out.print("Enter task name: ");
        String taskName = scanner.nextLine().trim();

        boolean result = controller.secureAuthorization(userId, taskName);

        System.out.println("\n✅ Authorization result: " + (result ? "GRANTED" : "DENIED"));
        System.out.println("  Using proper comparison operator (==)");
    }

    private static void showAssignmentExamples() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("ASSIGNMENT VS COMPARISON EXAMPLES");
        System.out.println("=".repeat(50));

        System.out.println("\n🔴 INSECURE (Assignment in conditions):");
        System.out.println("  // VULNERABLE: Single = assigns value");
        System.out.println("  boolean authorized = false;");
        System.out.println("  if (authorized = user.isAdmin()) {  // ALWAYS true!");
        System.out.println("      // This code will execute even if user is not admin");
        System.out.println("  }");

        System.out.println("\n  // VULNERABLE: Assignment in while loop");
        System.out.println("  int i = 0;");
        System.out.println("  while (i = getNextValue()) {  // Infinite loop!");
        System.out.println("      // Process item");
        System.out.println("  }");

        System.out.println("\n  // VULNERABLE: Assignment in for loop");
        System.out.println("  for (int i = 0; i = 10; i++) {  // Condition is assignment!");
        System.out.println("      // This will compile but logic is broken");
        System.out.println("  }");

        System.out.println("\n✅ SECURE (Proper comparisons):");
        System.out.println("  // SECURE: Double equals for comparison");
        System.out.println("  boolean authorized = false;");
        System.out.println("  if (authorized == user.isAdmin()) {");
        System.out.println("      // This only executes if values are equal");
        System.out.println("  }");

        System.out.println("\n  // SECURE: Yoda conditions");
        System.out.println("  if (true == user.isAdmin()) {  // Can't accidentally assign");
        System.out.println("      // Safe from assignment bugs");
        System.out.println("  }");

        System.out.println("\n  // SECURE: Separate assignment and check");
        System.out.println("  boolean isAdmin = user.isAdmin();  // Assignment first");
        System.out.println("  if (isAdmin == true) {  // Then compare");
        System.out.println("      // Clear and safe");
        System.out.println("  }");
    }

    private static void testMultiFactorAuth(Scanner scanner, SecureAccessController controller) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("MULTI-FACTOR AUTHENTICATION TEST");
        System.out.println("=".repeat(50));

        System.out.print("\nEnter user ID: ");
        String userId = scanner.nextLine().trim();

        System.out.print("Enter task name: ");
        String taskName = scanner.nextLine().trim();

        System.out.print("Enter 6-digit token: ");
        String token = scanner.nextLine().trim();

        boolean result = controller.secureMultiFactorAuth(userId, taskName, token);

        System.out.println("\n✅ MFA result: " + (result ? "SUCCESS" : "FAILED"));
    }

    private static void analyzeCodePatterns() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("CODE PATTERN ANALYSIS");
        System.out.println("=".repeat(50));

        // Sample code snippets to analyze
        String[] snippets = {
                "if (authorized = user.isAdmin()) { }",
                "while (count = getNext()) { }",
                "if (authorized == true) { }",
                "for (int i = 0; i = size; i++) { }",
                "boolean valid = (status = checkStatus());"
        };

        for (int i = 0; i < snippets.length; i++) {
            System.out.println("\nSnippet " + (i + 1) + ": " + snippets[i]);
            CodeAnalyzer.analyzeCode(snippets[i]);
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-481");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-481 VULNERABLE):");
        System.out.println("  • Using assignment (=) instead of comparison (==)");
        System.out.println("  • Assignment in if/while/for conditions");
        System.out.println("  • Unintended variable modification");
        System.out.println("  • Logic bypass vulnerabilities");
        System.out.println("  • Silent security failures");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Proper Comparison:");
        System.out.println("     • Using == for equality checks");
        System.out.println("     • Using Yoda conditions (constant on left)");
        System.out.println("     • Separate assignment from condition");

        System.out.println("\n  2. Code Review Practices:");
        System.out.println("     • Static analysis tools");
        System.out.println("     • Peer code reviews");
        System.out.println("     • Automated testing");

        System.out.println("\n  3. Language Features:");
        System.out.println("     • Final variables where possible");
        System.out.println("     • Boolean conditions with explicit true/false");
        System.out.println("     • Avoid side effects in conditions");

        System.out.println("\n📋 BEST PRACTICES FOR CWE-481:");
        System.out.println("  1. Never use assignment in conditional expressions");
        System.out.println("  2. Use Yoda conditions (if (true == x))");
        System.out.println("  3. Enable compiler warnings for assignment in conditions");
        System.out.println("  4. Use static analysis tools (FindBugs, PMD)");
        System.out.println("  5. Write unit tests for all conditions");
        System.out.println("  6. Separate assignment and checking");
        System.out.println("  7. Use final variables for constants");
        System.out.println("  8. Conduct thorough code reviews");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Always use == for comparison");
        System.out.println("  • Consider Yoda conditions for critical checks");
        System.out.println("  • Use IDE warnings to catch these errors");
        System.out.println("  • Write clear, readable code");
        System.out.println("  • Test edge cases thoroughly");

        System.out.println("\n📊 COMMON MISTAKES:");
        System.out.println("  • if (authorized = true)    // ALWAYS true");
        System.out.println("  • if (x = y)                // Assignment, not comparison");
        System.out.println("  • while (i = getValue())    // Potential infinite loop");
        System.out.println("  • for (int i=0; i=10; i++)  // Never exits");

        System.out.println("\n✓ CORRECT FORMS:");
        System.out.println("  • if (authorized == true)");
        System.out.println("  • if (x == y)");
        System.out.println("  • while ((i = getValue()) != null) // Explicit");
        System.out.println("  • for (int i=0; i<10; i++)");
    }
}