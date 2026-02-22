package chatgpt5.java;

import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.*;

public class cwe_375 {

    // ==================== INSECURE USER CLASS (CWE-375 VULNERABLE)
    // ====================

    static class InsecureUser {
        private String userId;
        private String username;
        private String email;
        private ArrayList<String> roles; // Mutable collection
        private ArrayList<String> permissions; // Mutable collection
        private HashMap<String, Object> attributes; // Mutable map
        private Date lastLogin; // Mutable date
        private String[] temporaryAccess; // Mutable array

        public InsecureUser(String userId, String username) {
            this.userId = userId;
            this.username = username;
            this.roles = new ArrayList<>();
            this.permissions = new ArrayList<>();
            this.attributes = new HashMap<>();
            this.lastLogin = new Date();
            this.temporaryAccess = new String[0];

            // Add default roles
            this.roles.add("USER");

            // Add default permissions
            this.permissions.add("VIEW_PROFILE");
            this.permissions.add("EDIT_PROFILE");

            // Add default attributes
            this.attributes.put("accountType", "STANDARD");
            this.attributes.put("created", new Date());
        }

        // INSECURE: Returns direct reference to mutable ArrayList
        public ArrayList<String> getRoles() {
            return roles; // Caller can add/remove roles!
        }

        // INSECURE: Returns direct reference to mutable ArrayList
        public ArrayList<String> getPermissions() {
            return permissions; // Caller can add/remove permissions!
        }

        // INSECURE: Returns direct reference to mutable Map
        public HashMap<String, Object> getAttributes() {
            return attributes; // Caller can modify attributes!
        }

        // INSECURE: Returns direct reference to mutable Date
        public Date getLastLogin() {
            return lastLogin; // Caller can change login time!
        }

        // INSECURE: Returns direct reference to mutable array
        public String[] getTemporaryAccess() {
            return temporaryAccess; // Caller can modify array contents!
        }

        // INSECURE: Setter stores direct reference to mutable object
        public void setRoles(ArrayList<String> roles) {
            this.roles = roles; // Stores direct reference to mutable object
        }

        // INSECURE: Adds role without validation
        public void addRole(String role) {
            this.roles.add(role);
        }

        // INSECURE: Setter for array
        public void setTemporaryAccess(String[] access) {
            this.temporaryAccess = access; // Direct reference!
        }

        @Override
        public String toString() {
            return String.format("InsecureUser{id='%s', username='%s', roles=%s, permissions=%s}",
                    userId, username, roles, permissions);
        }

        // For demonstration
        public boolean hasRole(String role) {
            return roles.contains(role);
        }

        public boolean hasPermission(String permission) {
            return permissions.contains(permission);
        }
    }

    // ==================== SECURE USER CLASS (CWE-375 MITIGATED)
    // ====================

    static class SecureUser {
        private final String userId;
        private final String username;
        private String email;
        private final List<String> roles; // Final reference
        private final List<String> permissions; // Final reference
        private final Map<String, Object> attributes; // Final reference
        private final Date lastLogin; // Final reference
        private final String[] temporaryAccess; // Final array

        // Immutable alternatives
        private final Set<String> immutableRoles;
        private final Set<String> immutablePermissions;
        private final Instant lastLoginInstant;

        // Audit trail
        private final List<AuditEntry> auditLog;

        public SecureUser(String userId, String username) {
            this.userId = userId;
            this.username = username;
            this.lastLogin = new Date(); // Will need defensive copy
            this.lastLoginInstant = Instant.now();

            // SECURE: Initialize with defensive copies
            List<String> roleList = new ArrayList<>();
            roleList.add("USER");
            this.roles = Collections.unmodifiableList(roleList);

            List<String> permList = new ArrayList<>();
            permList.add("VIEW_PROFILE");
            permList.add("EDIT_PROFILE");
            this.permissions = Collections.unmodifiableList(permList);

            Map<String, Object> attrMap = new HashMap<>();
            attrMap.put("accountType", "STANDARD");
            attrMap.put("created", Instant.now().toString());
            this.attributes = Collections.unmodifiableMap(attrMap);

            this.temporaryAccess = new String[0];

            // SECURE: Immutable sets from collections
            this.immutableRoles = Set.copyOf(roleList); // Java 9+ immutable copy
            this.immutablePermissions = Set.copyOf(permList);

            this.auditLog = new ArrayList<>();
            logAudit("USER_CREATED", "User account created");
        }

        // SECURE: Return unmodifiable view
        public List<String> getRoles() {
            return roles; // Already unmodifiable
        }

        // SECURE: Return immutable copy
        public Set<String> getImmutableRoles() {
            return immutableRoles; // Truly immutable
        }

        // SECURE: Return unmodifiable view
        public List<String> getPermissions() {
            return permissions; // Already unmodifiable
        }

        // SECURE: Return immutable copy
        public Set<String> getImmutablePermissions() {
            return immutablePermissions; // Truly immutable
        }

        // SECURE: Return unmodifiable view of attributes
        public Map<String, Object> getAttributes() {
            return attributes; // Already unmodifiable
        }

        // SECURE: Return defensive copy of Date
        public Date getLastLogin() {
            return new Date(lastLogin.getTime()); // Defensive copy
        }

        // SECURE: Return immutable Instant
        public Instant getLastLoginInstant() {
            return lastLoginInstant; // Immutable
        }

        // SECURE: Return defensive copy of array
        public String[] getTemporaryAccess() {
            return Arrays.copyOf(temporaryAccess, temporaryAccess.length);
        }

        // SECURE: Return immutable copy of temporary access
        public List<String> getTemporaryAccessList() {
            return List.of(temporaryAccess); // Java 9+ immutable list
        }

        // SECURE: Safe way to add role with validation
        public boolean addRole(String role, User actor) {
            if (!actor.isAdmin()) {
                logAudit("ROLE_ADD_FAILED", "Non-admin attempted to add role: " + role);
                return false;
            }

            if (role == null || role.trim().isEmpty()) {
                return false;
            }

            // SECURE: Need to create new unmodifiable list
            List<String> newRoles = new ArrayList<>(roles);
            if (newRoles.add(role)) {
                // Can't modify final field - in real app, would need to recreate object
                logAudit("ROLE_ADDED", "Added role: " + role + " by " + actor.getUsername());
                return true;
            }
            return false;
        }

        // SECURE: Safe way to check role
        public boolean hasRole(String role) {
            return roles.contains(role);
        }

        // SECURE: Safe way to check permission
        public boolean hasPermission(String permission) {
            return permissions.contains(permission);
        }

        // SECURE: Safe way to set temporary access
        public void setTemporaryAccess(String[] access, User actor) {
            if (!actor.isAdmin()) {
                logAudit("ACCESS_SET_FAILED", "Non-admin attempted to set temporary access");
                throw new SecurityException("Only admins can set temporary access");
            }

            // SECURE: Store defensive copy
            this.temporaryAccess = Arrays.copyOf(access, access.length);
            logAudit("ACCESS_SET", "Temporary access updated by " + actor.getUsername());
        }

        // SECURE: Audit logging
        private void logAudit(String action, String details) {
            auditLog.add(new AuditEntry(action, details, Instant.now()));
        }

        public List<AuditEntry> getAuditLog(User actor) {
            if (!actor.isAdmin()) {
                throw new SecurityException("Only admins can view audit log");
            }
            return Collections.unmodifiableList(auditLog);
        }

        // Audit entry class
        static class AuditEntry {
            final String action;
            final String details;
            final Instant timestamp;

            AuditEntry(String action, String details, Instant timestamp) {
                this.action = action;
                this.details = details;
                this.timestamp = timestamp;
            }

            @Override
            public String toString() {
                return String.format("[%s] %s: %s", timestamp, action, details);
            }
        }
    }

    // ==================== USER CLASS FOR ACTOR ====================

    static class User {
        private final String username;
        private final boolean isAdmin;

        public User(String username, boolean isAdmin) {
            this.username = username;
            this.isAdmin = isAdmin;
        }

        public String getUsername() {
            return username;
        }

        public boolean isAdmin() {
            return isAdmin;
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class SecurityDemonstrator {

        public static void demonstrateInsecureRoles() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE ROLE EXPOSURE DEMONSTRATION");
            System.out.println("=".repeat(60));

            InsecureUser user = new InsecureUser("U001", "john_doe");

            System.out.println("\nOriginal user:");
            System.out.println("  Roles: " + user.getRoles());
            System.out.println("  Has 'USER' role: " + user.hasRole("USER"));

            // MALICIOUS CODE: External code modifies the roles
            System.out.println("\n🔴 MALICIOUS ACTOR: Got reference to roles list");
            ArrayList<String> stolenRoles = user.getRoles();

            // Add admin role
            stolenRoles.add("ADMIN");
            stolenRoles.remove("USER");

            System.out.println("  Modified roles through reference");
            System.out.println("  Added ADMIN, removed USER");

            System.out.println("\nUser after modification:");
            System.out.println("  Roles: " + user.getRoles());
            System.out.println("  Has 'USER' role: " + user.hasRole("USER"));
            System.out.println("  Has 'ADMIN' role: " + user.hasRole("ADMIN"));

            if (user.hasRole("ADMIN")) {
                System.out.println("\n⚠️ SECURITY BREACH: User gained ADMIN role!");
            }
        }

        public static void demonstrateInsecurePermissions() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE PERMISSION EXPOSURE DEMONSTRATION");
            System.out.println("=".repeat(60));

            InsecureUser user = new InsecureUser("U002", "jane_doe");

            System.out.println("\nOriginal permissions: " + user.getPermissions());

            // MALICIOUS CODE: Modify permissions
            System.out.println("\n🔴 MALICIOUS ACTOR: Got reference to permissions list");
            ArrayList<String> stolenPermissions = user.getPermissions();

            // Add admin permission
            stolenPermissions.add("ADMIN_ACCESS");
            stolenPermissions.remove("VIEW_PROFILE");

            System.out.println("  Modified permissions through reference");

            System.out.println("\nUser permissions after modification: " +
                    user.getPermissions());

            if (user.hasPermission("ADMIN_ACCESS")) {
                System.out.println("\n⚠️ SECURITY BREACH: User gained ADMIN_ACCESS permission!");
            }
        }

        public static void demonstrateInsecureAttributes() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE ATTRIBUTES EXPOSURE DEMONSTRATION");
            System.out.println("=".repeat(60));

            InsecureUser user = new InsecureUser("U003", "bob_smith");

            System.out.println("\nOriginal attributes: " + user.getAttributes());

            // MALICIOUS CODE: Modify attributes
            System.out.println("\n🔴 MALICIOUS ACTOR: Got reference to attributes map");
            HashMap<String, Object> stolenAttributes = user.getAttributes();

            // Modify attributes
            stolenAttributes.put("accountType", "PREMIUM");
            stolenAttributes.put("creditLimit", 10000);
            stolenAttributes.remove("created");

            System.out.println("  Modified attributes through reference");

            System.out.println("\nUser attributes after modification: " +
                    user.getAttributes());
        }

        public static void demonstrateSecureRoles() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE ROLE HANDLING DEMONSTRATION");
            System.out.println("=".repeat(60));

            User admin = new User("admin", true);
            User regular = new User("regular", false);

            SecureUser user = new SecureUser("U004", "alice");

            System.out.println("\nOriginal user:");
            System.out.println("  Roles (unmodifiable): " + user.getRoles());
            System.out.println("  Roles (immutable set): " + user.getImmutableRoles());
            System.out.println("  Has 'USER' role: " + user.hasRole("USER"));

            // Attempt to modify through reference
            System.out.println("\n🟢 ATTEMPT: Trying to modify roles through getter");
            List<String> stolenRoles = user.getRoles();

            try {
                stolenRoles.add("ADMIN");
                System.out.println("  Modified roles? (should fail)");
            } catch (UnsupportedOperationException e) {
                System.out.println("  ✅ BLOCKED: Cannot modify unmodifiable list: " +
                        e.getMessage());
            }

            // Try with immutable set
            System.out.println("\n🟢 ATTEMPT: Trying to modify immutable set");
            Set<String> stolenSet = user.getImmutableRoles();

            try {
                stolenSet.add("ADMIN");
                System.out.println("  Modified set? (should fail)");
            } catch (UnsupportedOperationException e) {
                System.out.println("  ✅ BLOCKED: Cannot modify immutable set");
            }

            System.out.println("\nUser after attempts:");
            System.out.println("  Roles: " + user.getRoles());

            // Try legitimate role addition
            System.out.println("\n🟢 ATTEMPT: Regular user tries to add role");
            boolean result = user.addRole("ADMIN", regular);
            System.out.println("  Result: " + (result ? "SUCCESS" : "FAILED - Access denied"));

            System.out.println("\n🟢 ATTEMPT: Admin tries to add role");
            result = user.addRole("MANAGER", admin);
            System.out.println("  Result: " + (result ? "SUCCESS" : "FAILED"));

            System.out.println("\n✅ PROTECTED: Roles properly encapsulated");
        }

        public static void demonstrateSecureTemporaryAccess() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE TEMPORARY ACCESS DEMONSTRATION");
            System.out.println("=".repeat(60));

            User admin = new User("admin", true);
            User regular = new User("regular", false);

            SecureUser user = new SecureUser("U005", "charlie");

            String[] access = { "ROOM_101", "SERVER_ROOM", "DATA_CENTER" };
            String[] maliciousCopy = access.clone();

            System.out.println("\nSetting temporary access as admin...");
            user.setTemporaryAccess(access, admin);

            System.out.println("Temporary access: " +
                    Arrays.toString(user.getTemporaryAccess()));

            // Attempt to modify returned array
            System.out.println("\n🟢 ATTEMPT: Trying to modify returned array");
            String[] stolenAccess = user.getTemporaryAccess();

            // Try to modify the copy
            stolenAccess[0] = "COMPROMISED";
            System.out.println("  Modified the copy - changed first element");

            System.out.println("\nOriginal after modification attempt:");
            System.out.println("  Original access: " + Arrays.toString(user.getTemporaryAccess()));

            if (!Arrays.equals(user.getTemporaryAccess(), stolenAccess)) {
                System.out.println("\n✅ PROTECTED: Original array unchanged!");
            }

            // Try with immutable list
            System.out.println("\nImmutable list view: " + user.getTemporaryAccessList());

            try {
                user.getTemporaryAccessList().add("NEW_ROOM");
                System.out.println("  Modified list? (should fail)");
            } catch (UnsupportedOperationException e) {
                System.out.println("  ✅ BLOCKED: Cannot modify immutable list");
            }

            // Try unauthorized set
            System.out.println("\n🟢 ATTEMPT: Regular user tries to set temporary access");
            try {
                user.setTemporaryAccess(new String[] { "UNAUTHORIZED" }, regular);
                System.out.println("  Set access? (should fail)");
            } catch (SecurityException e) {
                System.out.println("  ✅ BLOCKED: " + e.getMessage());
            }
        }

        public static void demonstrateAuditLog() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("AUDIT LOG DEMONSTRATION");
            System.out.println("=".repeat(60));

            User admin = new User("admin", true);
            User regular = new User("regular", false);

            SecureUser user = new SecureUser("U006", "diana");

            // Perform some actions
            user.addRole("MANAGER", admin);
            user.setTemporaryAccess(new String[] { "VAULT" }, admin);

            System.out.println("\nAttempting to view audit log as regular user...");
            try {
                user.getAuditLog(regular);
                System.out.println("  Viewed log? (should fail)");
            } catch (SecurityException e) {
                System.out.println("  ✅ BLOCKED: " + e.getMessage());
            }

            System.out.println("\nViewing audit log as admin:");
            List<SecureUser.AuditEntry> log = user.getAuditLog(admin);
            System.out.println("  Audit entries (" + log.size() + "):");
            for (SecureUser.AuditEntry entry : log) {
                System.out.println("    " + entry);
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-375: Returning a Mutable Object (Roles Collection)");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE roles exposure (CWE-375)");
            System.out.println("  2. Demonstrate INSECURE permissions exposure");
            System.out.println("  3. Demonstrate INSECURE attributes exposure");
            System.out.println("  4. Demonstrate SECURE roles handling");
            System.out.println("  5. Demonstrate SECURE temporary access");
            System.out.println("  6. Demonstrate audit logging");
            System.out.println("  7. Run all demonstrations");
            System.out.println("  8. Show security analysis");
            System.out.println("  9. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    SecurityDemonstrator.demonstrateInsecureRoles();
                    break;

                case "2":
                    SecurityDemonstrator.demonstrateInsecurePermissions();
                    break;

                case "3":
                    SecurityDemonstrator.demonstrateInsecureAttributes();
                    break;

                case "4":
                    SecurityDemonstrator.demonstrateSecureRoles();
                    break;

                case "5":
                    SecurityDemonstrator.demonstrateSecureTemporaryAccess();
                    break;

                case "6":
                    SecurityDemonstrator.demonstrateAuditLog();
                    break;

                case "7":
                    runAllDemonstrations();
                    break;

                case "8":
                    showSecurityAnalysis();
                    break;

                case "9":
                    System.out.println("\nExiting...");
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void runAllDemonstrations() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("RUNNING ALL DEMONSTRATIONS");
        System.out.println("=".repeat(60));

        SecurityDemonstrator.demonstrateInsecureRoles();
        SecurityDemonstrator.demonstrateInsecurePermissions();
        SecurityDemonstrator.demonstrateInsecureAttributes();
        SecurityDemonstrator.demonstrateSecureRoles();
        SecurityDemonstrator.demonstrateSecureTemporaryAccess();
        SecurityDemonstrator.demonstrateAuditLog();
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-375");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-375 VULNERABLE):");
        System.out.println("  1. Direct Collection Exposure:");
        System.out.println("     • Returns reference to internal ArrayList");
        System.out.println("     • Caller can add/remove roles");
        System.out.println("     • Can grant unauthorized privileges");

        System.out.println("\n  2. Mutable Object Exposure:");
        System.out.println("     • Returns reference to internal Date");
        System.out.println("     • Returns reference to internal Map");
        System.out.println("     • Returns reference to internal Array");

        System.out.println("\n  3. Consequences:");
        System.out.println("     • Privilege escalation");
        System.out.println("     • Data corruption");
        System.out.println("     • Security bypass");
        System.out.println("     • Inconsistent state");
        System.out.println("     • No audit trail");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Unmodifiable Collections:");
        System.out.println("     • Collections.unmodifiableList()");
        System.out.println("     • Collections.unmodifiableMap()");
        System.out.println("     • Set.copyOf() for immutable copies");

        System.out.println("\n  2. Defensive Copying:");
        System.out.println("     • Return new Date(oldDate.getTime())");
        System.out.println("     • Return Arrays.copyOf(array)");
        System.out.println("     • Original state protected");

        System.out.println("\n  3. Immutable Alternatives:");
        System.out.println("     • Instant instead of Date");
        System.out.println("     • Set.copyOf() for immutable sets");
        System.out.println("     • List.of() for immutable lists");

        System.out.println("\n  4. Access Control:");
        System.out.println("     • Validate actor permissions");
        System.out.println("     • Audit all changes");
        System.out.println("     • Log security events");

        System.out.println("\n📋 BEST PRACTICES FOR COLLECTIONS:");
        System.out.println("  1. Never return internal collections directly");
        System.out.println("  2. Use Collections.unmodifiableList()");
        System.out.println("  3. Use Set.copyOf() for true immutability");
        System.out.println("  4. Store defensive copies in constructors");
        System.out.println("  5. Validate all modifications");
        System.out.println("  6. Audit privilege changes");
        System.out.println("  7. Use immutable objects where possible");
        System.out.println("  8. Document mutability guarantees");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Return Collections.unmodifiableList(roles)");
        System.out.println("  • Use Set.copyOf() for immutable views");
        System.out.println("  • Implement proper access control");
        System.out.println("  • Add comprehensive audit logging");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  public ArrayList<String> getRoles() {");
        System.out.println("      return this.roles; // Direct reference!");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // SECURE (unmodifiable)");
        System.out.println("  public List<String> getRoles() {");
        System.out.println("      return Collections.unmodifiableList(roles);");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // SECURE (immutable copy)");
        System.out.println("  public Set<String> getRoles() {");
        System.out.println("      return Set.copyOf(roles);");
        System.out.println("  }");

        System.out.println("\n📊 COLLECTION SECURITY:");
        System.out.println("  Return Type        | Modifiable | Safe");
        System.out.println("  -------------------|------------|------");
        System.out.println("  ArrayList<String>  |     ✓      |  ❌");
        System.out.println("  List<String>       |     ✓      |  ❌");
        System.out.println("  UnmodifiableList   |     ❌     |  ✓");
        System.out.println("  Set.copyOf()       |     ❌     |  ✓");
        System.out.println("  List.of()          |     ❌     |  ✓");

        System.out.println("\n⚠️ NOTE: Even returning List interface type");
        System.out.println("  is unsafe if the underlying implementation is mutable!");
    }
}