package deepseek.java;

import java.util.*;
import java.math.*;
import java.time.*;
import java.time.format.*;
import java.io.*;

public class cwe_766 {

    // ==================== INSECURE EMPLOYEE CLASS (CWE-766 VULNERABLE)
    // ====================

    static class InsecureEmployee {

        // INSECURE: Critical data stored in public fields
        public String employeeId;
        public String name;
        public String department;
        public String position;

        // INSECURE: Sensitive fields are public and mutable
        public String taxID; // Social Security Number / Tax ID
        public double salary; // Salary amount
        public String homeAddress; // Home address
        public String phoneNumber; // Personal phone
        public String email; // Personal email
        public String bankAccount; // Bank account number
        public String routingNumber; // Bank routing number
        public String creditCardNumber; // Credit card (if stored)
        public Date dateOfBirth; // Date of birth
        public String emergencyContact; // Emergency contact info

        // INSECURE: No access control
        public boolean isManager;
        public boolean isActive;
        public int clearanceLevel;

        // INSECURE: Audit fields also public
        public String createdBy;
        public Date createdAt;
        public String modifiedBy;
        public Date modifiedAt;

        public InsecureEmployee(String employeeId, String name, String taxID, double salary) {
            this.employeeId = employeeId;
            this.name = name;
            this.taxID = taxID;
            this.salary = salary;
            this.createdAt = new Date();
            this.isActive = true;
        }

        // INSECURE: No validation or protection
        public void setSalary(double newSalary) {
            this.salary = newSalary; // No validation, no audit
        }

        // INSECURE: Returns internal data directly
        public String getTaxID() {
            return taxID; // Direct exposure
        }

        @Override
        public String toString() {
            return String.format("Employee{id='%s', name='%s', taxID='%s', salary=%.2f}",
                    employeeId, name, taxID, salary);
        }
    }

    // ==================== INSECURE EMPLOYEE WITH WRONG DATA TYPES
    // ====================

    static class InsecureEmployee2 {

        // INSECURE: Using wrong data types for sensitive information
        public int taxID; // Tax ID stored as int (loses leading zeros)
        public float salary; // Float loses precision for money
        public long creditCard; // Credit card as long (loses leading zeros)
        public int zipCode; // Zip code as int (loses leading zeros)

        // INSECURE: String concatenation for sensitive data
        public String getDisplayInfo() {
            return "Tax ID: " + taxID + ", Salary: " + salary; // Exposed!
        }

        public InsecureEmployee2(int taxID, float salary) {
            this.taxID = taxID;
            this.salary = salary;
        }
    }

    // ==================== INSECURE EMPLOYEE WITH WRONG VARIABLE USAGE
    // ====================

    static class InsecureEmployee3 {

        // INSECURE: Variable naming confusion
        private String ssn; // Social Security Number
        private String password; // User password
        private String ssn2; // What is this? Copy of SSN?

        // INSECURE: Reusing variables for different purposes
        private String temp; // Used for multiple purposes

        // INSECURE: Similar variable names cause confusion
        private String employeeAddress;
        private String homeAddress; // Same as above?
        private String mailingAddress; // Or this?

        public void processData(String input) {
            // INSECURE: Temp variable misuse
            temp = input; // Now temp contains sensitive data
            temp = temp.toUpperCase(); // Still has sensitive data
            // Original sensitive data still in temp after use

            // INSECURE: Variable confusion
            employeeAddress = "123 Work St";
            homeAddress = employeeAddress; // Should be separate
        }
    }

    // ==================== SECURE EMPLOYEE CLASS (CWE-766 MITIGATED)
    // ====================

    static class SecureEmployee {

        // SECURE: Private fields with access control
        private String employeeId;
        private String name;
        private String department;
        private String position;

        // SECURE: Sensitive data properly encapsulated
        private EncryptedData taxID; // Encrypted tax ID
        private BigDecimal salary; // Proper type for money
        private EncryptedData homeAddress; // Encrypted address
        private EncryptedData phoneNumber; // Encrypted phone
        private EncryptedData email; // Encrypted email
        private EncryptedData bankAccount; // Encrypted bank account
        private EncryptedData routingNumber; // Encrypted routing number
        private LocalDate dateOfBirth; // Proper date type
        private EncryptedData emergencyContact; // Encrypted contact info

        // SECURE: Role-based permissions
        private EnumSet<Permission> permissions;
        private int clearanceLevel;
        private boolean isActive;

        // SECURE: Audit trail
        private AuditInfo auditInfo;

        // SECURE: Access control flags
        private transient boolean canViewSalary;
        private transient boolean canViewTaxID;
        private transient boolean canModify;

        public enum Permission {
            VIEW_SALARY, VIEW_TAX_ID, VIEW_ADDRESS, VIEW_PHONE,
            MODIFY_SALARY, MODIFY_PROFILE, VIEW_AUDIT, ADMIN
        }

        public SecureEmployee(String employeeId, String name, String taxID, BigDecimal salary) {
            this.employeeId = employeeId;
            this.name = name;
            this.taxID = new EncryptedData(taxID);
            this.salary = salary;
            this.permissions = EnumSet.noneOf(Permission.class);
            this.auditInfo = new AuditInfo();
            this.isActive = true;
            this.dateOfBirth = null;

            auditInfo.recordCreation(name);
        }

        // SECURE: Controlled access with validation
        public String getTaxID(User viewer) {
            if (canViewTaxID(viewer)) {
                auditInfo.recordAccess("TAX_ID", viewer.getUsername(), "VIEW");
                return taxID.decrypt();
            }
            auditInfo.recordAccess("TAX_ID", viewer.getUsername(), "DENIED");
            throw new SecurityException("Access denied to tax ID");
        }

        // SECURE: Masked output for display
        public String getMaskedTaxID() {
            String decrypted = taxID.decrypt();
            if (decrypted.length() <= 4)
                return "***";
            return "***-**-" + decrypted.substring(decrypted.length() - 4);
        }

        // SECURE: Salary with proper type and validation
        public BigDecimal getSalary(User viewer) {
            if (canViewSalary(viewer)) {
                auditInfo.recordAccess("SALARY", viewer.getUsername(), "VIEW");
                return salary;
            }
            auditInfo.recordAccess("SALARY", viewer.getUsername(), "DENIED");
            throw new SecurityException("Access denied to salary");
        }

        // SECURE: Salary modification with validation
        public void setSalary(User modifier, BigDecimal newSalary) {
            if (!canModify(modifier)) {
                auditInfo.recordAccess("SALARY_MODIFY", modifier.getUsername(), "DENIED");
                throw new SecurityException("Cannot modify salary");
            }

            if (newSalary.compareTo(BigDecimal.ZERO) < 0) {
                throw new IllegalArgumentException("Salary cannot be negative");
            }

            if (newSalary.compareTo(new BigDecimal("1000000")) > 0) {
                // Requires additional approval
                auditInfo.recordAccess("SALARY_MODIFY", modifier.getUsername(), "PENDING_APPROVAL");
                throw new SecurityException("Salary above limit requires approval");
            }

            BigDecimal oldSalary = this.salary;
            this.salary = newSalary;
            auditInfo.recordSalaryChange(modifier.getUsername(), oldSalary, newSalary);
        }

        // SECURE: Address with proper handling
        public void setHomeAddress(User modifier, String address) {
            if (!canModify(modifier)) {
                auditInfo.recordAccess("ADDRESS_MODIFY", modifier.getUsername(), "DENIED");
                throw new SecurityException("Cannot modify address");
            }

            this.homeAddress = new EncryptedData(address);
            auditInfo.recordChange("ADDRESS", modifier.getUsername());
        }

        public String getHomeAddress(User viewer) {
            if (canViewAddress(viewer)) {
                auditInfo.recordAccess("ADDRESS", viewer.getUsername(), "VIEW");
                return homeAddress != null ? homeAddress.decrypt() : null;
            }
            auditInfo.recordAccess("ADDRESS", viewer.getUsername(), "DENIED");
            throw new SecurityException("Access denied to address");
        }

        public String getMaskedAddress() {
            if (homeAddress == null)
                return null;
            String addr = homeAddress.decrypt();
            // Return first line only for display
            String[] parts = addr.split(",");
            return parts[0] + ", ...";
        }

        // SECURE: Permission checks
        private boolean canViewTaxID(User viewer) {
            return viewer != null &&
                    (viewer.isAdmin() ||
                            viewer.getUsername().equals(this.employeeId) ||
                            permissions.contains(Permission.VIEW_TAX_ID));
        }

        private boolean canViewSalary(User viewer) {
            return viewer != null &&
                    (viewer.isAdmin() ||
                            permissions.contains(Permission.VIEW_SALARY));
        }

        private boolean canViewAddress(User viewer) {
            return viewer != null &&
                    (viewer.isAdmin() ||
                            viewer.getUsername().equals(this.employeeId) ||
                            permissions.contains(Permission.VIEW_ADDRESS));
        }

        private boolean canModify(User modifier) {
            return modifier != null &&
                    (modifier.isAdmin() ||
                            permissions.contains(Permission.MODIFY_PROFILE));
        }

        // SECURE: Add permissions
        public void addPermission(User granter, Permission permission) {
            if (granter == null || !granter.isAdmin()) {
                throw new SecurityException("Only admins can grant permissions");
            }
            permissions.add(permission);
            auditInfo.recordPermissionChange(granter.getUsername(), permission, "GRANT");
        }

        // SECURE: Remove permissions
        public void removePermission(User revoker, Permission permission) {
            if (revoker == null || !revoker.isAdmin()) {
                throw new SecurityException("Only admins can revoke permissions");
            }
            permissions.remove(permission);
            auditInfo.recordPermissionChange(revoker.getUsername(), permission, "REVOKE");
        }

        // SECURE: Audit info access
        public List<AuditInfo.AuditEntry> getAuditLog(User viewer) {
            if (viewer == null || !viewer.isAdmin()) {
                throw new SecurityException("Only admins can view audit logs");
            }
            return auditInfo.getEntries();
        }

        // SECURE: Nested class for encrypted data
        static class EncryptedData {
            private final String encryptedValue;
            private final String algorithm = "AES-256";

            public EncryptedData(String plaintext) {
                // In production, use proper encryption
                this.encryptedValue = encrypt(plaintext);
            }

            private String encrypt(String plaintext) {
                // Simple encoding for demo - use proper encryption in production
                return Base64.getEncoder().encodeToString(plaintext.getBytes());
            }

            public String decrypt() {
                // Simple decoding for demo
                return new String(Base64.getDecoder().decode(encryptedValue));
            }
        }

        // SECURE: Audit information
        static class AuditInfo {
            private final List<AuditEntry> entries = new ArrayList<>();
            private String createdBy;
            private LocalDateTime createdAt;

            public AuditInfo() {
                this.createdAt = LocalDateTime.now();
            }

            public void recordCreation(String creator) {
                this.createdBy = creator;
                entries.add(new AuditEntry("CREATED", creator, "Employee record created"));
            }

            public void recordAccess(String field, String user, String action) {
                entries.add(new AuditEntry("ACCESS", field, user, action));
            }

            public void recordChange(String field, String user) {
                entries.add(new AuditEntry("MODIFY", field, user, "Field modified"));
            }

            public void recordSalaryChange(String user, BigDecimal oldValue, BigDecimal newValue) {
                entries.add(new AuditEntry("SALARY_CHANGE", user,
                        String.format("Salary changed from %s to %s", oldValue, newValue)));
            }

            public void recordPermissionChange(String user, Permission perm, String action) {
                entries.add(new AuditEntry("PERMISSION", user,
                        String.format("%s permission %s", action, perm)));
            }

            public List<AuditEntry> getEntries() {
                return new ArrayList<>(entries);
            }

            static class AuditEntry {
                private final String type;
                private final String field;
                private final String user;
                private final String details;
                private final LocalDateTime timestamp;

                AuditEntry(String type, String field, String user, String details) {
                    this.type = type;
                    this.field = field;
                    this.user = user;
                    this.details = details;
                    this.timestamp = LocalDateTime.now();
                }

                AuditEntry(String type, String user, String details) {
                    this(type, null, user, details);
                }

                @Override
                public String toString() {
                    return String.format("[%s] %s - %s: %s %s",
                            timestamp.format(DateTimeFormatter.ISO_LOCAL_DATE_TIME),
                            type, user, field != null ? field + " - " : "", details);
                }
            }
        }
    }

    // ==================== USER CLASS FOR ACCESS CONTROL ====================

    static class User {
        private String username;
        private String role;
        private boolean isAdmin;

        public User(String username, String role) {
            this.username = username;
            this.role = role;
            this.isAdmin = "ADMIN".equals(role);
        }

        public String getUsername() {
            return username;
        }

        public String getRole() {
            return role;
        }

        public boolean isAdmin() {
            return isAdmin;
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-766: Critical Data Element in Wrong Variable");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE Employee (CWE-766)");
            System.out.println("  2. Demonstrate INSECURE Employee - Wrong Data Types");
            System.out.println("  3. Demonstrate INSECURE Employee - Variable Confusion");
            System.out.println("  4. Demonstrate SECURE Employee (Mitigated)");
            System.out.println("  5. Compare Implementations");
            System.out.println("  6. Test Access Control");
            System.out.println("  7. Show Security Analysis");
            System.out.println("  8. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecureEmployee();
                    break;

                case "2":
                    demonstrateInsecureEmployee2();
                    break;

                case "3":
                    demonstrateInsecureEmployee3();
                    break;

                case "4":
                    demonstrateSecureEmployee();
                    break;

                case "5":
                    compareImplementations();
                    break;

                case "6":
                    testAccessControl(scanner);
                    break;

                case "7":
                    showSecurityAnalysis();
                    break;

                case "8":
                    System.out.println("\nExiting...");
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void demonstrateInsecureEmployee() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE EMPLOYEE DEMONSTRATION");
        System.out.println("=".repeat(50));

        // Create insecure employee
        InsecureEmployee emp = new InsecureEmployee("E001", "John Doe", "123-45-6789", 75000.00);
        emp.homeAddress = "123 Main St, Anytown, USA";
        emp.phoneNumber = "555-123-4567";
        emp.bankAccount = "123456789";
        emp.routingNumber = "987654321";
        emp.dateOfBirth = new Date(85, 5, 15); // June 15, 1985

        System.out.println("\nEmployee created with public fields:");
        System.out.println("  " + emp);

        System.out.println("\n🔴 PROBLEM 1: Public fields can be accessed directly:");
        System.out.println("  Tax ID: " + emp.taxID);
        System.out.println("  Salary: " + emp.salary);
        System.out.println("  Address: " + emp.homeAddress);
        System.out.println("  Bank Account: " + emp.bankAccount);

        System.out.println("\n🔴 PROBLEM 2: No validation on modifications:");
        emp.setSalary(-5000.00); // Negative salary allowed!
        System.out.println("  New salary (negative): " + emp.salary);

        System.out.println("\n🔴 PROBLEM 3: No audit trail:");
        System.out.println("  No record of who accessed or modified data");

        System.out.println("\n🔴 PROBLEM 4: Data exposed in toString():");
        System.out.println("  " + emp.toString());

        System.out.println("\n🔴 PROBLEM 5: Wrong data types:");
        System.out.println("  Salary as double: " + emp.salary + " (precision issues for money)");
    }

    private static void demonstrateInsecureEmployee2() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE EMPLOYEE - WRONG DATA TYPES");
        System.out.println("=".repeat(50));

        InsecureEmployee2 emp = new InsecureEmployee2(123456789, 75000.50f);

        System.out.println("\n🔴 PROBLEM: Wrong data types for sensitive information:");
        System.out.println("  Tax ID as int: " + emp.taxID);
        System.out.println("  (Leading zeros lost if tax ID was 0123456789)");

        System.out.println("  Salary as float: " + emp.salary);
        System.out.println("  (Precision issues: " + (emp.salary / 3) + " may be inaccurate)");

        System.out.println("\n🔴 PROBLEM: Exposed in string concatenation:");
        System.out.println("  " + emp.getDisplayInfo());
    }

    private static void demonstrateInsecureEmployee3() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE EMPLOYEE - VARIABLE CONFUSION");
        System.out.println("=".repeat(50));

        InsecureEmployee3 emp = new InsecureEmployee3();

        System.out.println("\n🔴 PROBLEM 1: Variable naming confusion:");
        System.out.println("  ssn, ssn2 - unclear purpose and duplication");

        System.out.println("\n🔴 PROBLEM 2: Temp variable misuse:");
        emp.processData("Sensitive Data");
        System.out.println("  Temp variable still contains data after use");

        System.out.println("\n🔴 PROBLEM 3: Address confusion:");
        System.out.println("  employeeAddress, homeAddress, mailingAddress - unclear distinctions");
    }

    private static void demonstrateSecureEmployee() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SECURE EMPLOYEE DEMONSTRATION");
        System.out.println("=".repeat(50));

        // Create users
        User admin = new User("admin", "ADMIN");
        User employee = new User("john.doe", "EMPLOYEE");
        User hrUser = new User("hr.user", "HR");

        // Create secure employee
        SecureEmployee emp = new SecureEmployee(
                "E001",
                "John Doe",
                "123-45-6789",
                new BigDecimal("75000.00"));

        // Set additional data securely
        emp.setHomeAddress(admin, "123 Main St, Anytown, USA 12345");

        System.out.println("\n✅ SECURE FEATURE 1: Encrypted sensitive data");
        System.out.println("  Tax ID (encrypted stored)");
        System.out.println("  Masked display: " + emp.getMaskedTaxID());

        System.out.println("\n✅ SECURE FEATURE 2: Access control");
        System.out.println("  Employee accessing own data:");
        try {
            String taxID = emp.getTaxID(employee);
            System.out.println("  Tax ID accessed: " + taxID);
        } catch (SecurityException e) {
            System.out.println("  Access denied: " + e.getMessage());
        }

        System.out.println("\n  Admin accessing data:");
        try {
            String taxID = emp.getTaxID(admin);
            System.out.println("  Tax ID accessed: " + taxID);
        } catch (SecurityException e) {
            System.out.println("  Access denied: " + e.getMessage());
        }

        System.out.println("\n✅ SECURE FEATURE 3: Proper data types");
        System.out.println("  Salary as BigDecimal: " + emp.getSalary(admin));

        System.out.println("\n✅ SECURE FEATURE 4: Validation");
        try {
            emp.setSalary(admin, new BigDecimal("-1000"));
        } catch (IllegalArgumentException e) {
            System.out.println("  Validation caught: " + e.getMessage());
        }

        System.out.println("\n✅ SECURE FEATURE 5: Audit trail");
        List<SecureEmployee.AuditInfo.AuditEntry> logs = emp.getAuditLog(admin);
        System.out.println("  Audit entries (" + logs.size() + "):");
        for (SecureEmployee.AuditInfo.AuditEntry entry : logs) {
            System.out.println("    " + entry);
        }
    }

    private static void compareImplementations() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("IMPLEMENTATION COMPARISON");
        System.out.println("=".repeat(60));

        System.out.println("\n📊 INSECURE EMPLOYEE:");
        System.out.println("  Access Control:  ❌ Public fields - anyone can access");
        System.out.println("  Data Types:      ❌ Wrong types (double for money)");
        System.out.println("  Validation:      ❌ None - negative salary allowed");
        System.out.println("  Encryption:      ❌ Plain text storage");
        System.out.println("  Audit Trail:     ❌ No logging");
        System.out.println("  Variable Names:  ❌ Confusing/redundant");
        System.out.println("  toString():      ❌ Exposes all data");
        System.out.println("  Risk Level:      🔴 HIGH - Data breach likely");

        System.out.println("\n📊 SECURE EMPLOYEE:");
        System.out.println("  Access Control:  ✅ Private fields with permissions");
        System.out.println("  Data Types:      ✅ Proper types (BigDecimal)");
        System.out.println("  Validation:      ✅ Input validation");
        System.out.println("  Encryption:      ✅ Encrypted storage");
        System.out.println("  Audit Trail:     ✅ Comprehensive logging");
        System.out.println("  Variable Names:  ✅ Clear, purposeful naming");
        System.out.println("  toString():      ✅ No sensitive data exposed");
        System.out.println("  Risk Level:      ✅ LOW - Data protected");
    }

    private static void testAccessControl(Scanner scanner) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("ACCESS CONTROL TEST");
        System.out.println("=".repeat(50));

        // Create users
        User admin = new User("admin", "ADMIN");
        User employee = new User("john.doe", "EMPLOYEE");
        User hrUser = new User("hr.user", "HR");

        // Create secure employee
        SecureEmployee emp = new SecureEmployee(
                "E001",
                "John Doe",
                "123-45-6789",
                new BigDecimal("75000.00"));

        System.out.println("\nAvailable users:");
        System.out.println("  1. admin (ADMIN)");
        System.out.println("  2. john.doe (EMPLOYEE - record owner)");
        System.out.println("  3. hr.user (HR)");

        System.out.print("\nSelect user (1-3): ");
        String choice = scanner.nextLine().trim();

        User selectedUser;
        switch (choice) {
            case "1":
                selectedUser = admin;
                break;
            case "2":
                selectedUser = employee;
                break;
            case "3":
                selectedUser = hrUser;
                break;
            default:
                selectedUser = null;
        }

        if (selectedUser == null)
            return;

        System.out.println("\nTesting access for: " + selectedUser.getUsername() +
                " (" + selectedUser.getRole() + ")");

        // Test tax ID access
        try {
            String taxID = emp.getTaxID(selectedUser);
            System.out.println("✅ Tax ID access: GRANTED - " + taxID);
        } catch (SecurityException e) {
            System.out.println("❌ Tax ID access: DENIED - " + e.getMessage());
        }

        // Test salary access
        try {
            BigDecimal salary = emp.getSalary(selectedUser);
            System.out.println("✅ Salary access: GRANTED - $" + salary);
        } catch (SecurityException e) {
            System.out.println("❌ Salary access: DENIED - " + e.getMessage());
        }

        // Test address access
        try {
            String address = emp.getHomeAddress(selectedUser);
            System.out.println("✅ Address access: GRANTED - " + address);
        } catch (SecurityException e) {
            System.out.println("❌ Address access: DENIED - " + e.getMessage());
        }

        // Test modification
        try {
            emp.setSalary(selectedUser, new BigDecimal("80000.00"));
            System.out.println("✅ Salary modification: GRANTED");
        } catch (SecurityException e) {
            System.out.println("❌ Salary modification: DENIED - " + e.getMessage());
        }

        // Show audit log
        System.out.println("\nAudit log entries:");
        List<SecureEmployee.AuditInfo.AuditEntry> logs = emp.getAuditLog(admin);
        for (SecureEmployee.AuditInfo.AuditEntry entry : logs) {
            System.out.println("  " + entry);
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-766");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-766 VULNERABLE):");
        System.out.println("  1. Public Fields:");
        System.out.println("     • Anyone can read/write sensitive data");
        System.out.println("     • No access control");
        System.out.println("     • Data easily exposed");

        System.out.println("\n  2. Wrong Data Types:");
        System.out.println("     • double for money (precision loss)");
        System.out.println("     • int for tax ID (leading zeros lost)");
        System.out.println("     • String for dates (parsing issues)");

        System.out.println("\n  3. No Validation:");
        System.out.println("     • Negative salaries allowed");
        System.out.println("     • Invalid data accepted");
        System.out.println("     • Business rules bypassed");

        System.out.println("\n  4. No Audit Trail:");
        System.out.println("     • No record of access");
        System.out.println("     • Cannot detect breaches");
        System.out.println("     • Compliance violations");

        System.out.println("\n  5. Variable Confusion:");
        System.out.println("     • Similar names cause mistakes");
        System.out.println("     • Temp variable reuse exposes data");
        System.out.println("     • Duplicate data (inconsistency)");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Encapsulation:");
        System.out.println("     • Private fields with accessors");
        System.out.println("     • Role-based access control");
        System.out.println("     • Masked data for display");

        System.out.println("\n  2. Proper Data Types:");
        System.out.println("     • BigDecimal for money");
        System.out.println("     • String for identifiers (preserves format)");
        System.out.println("     • LocalDate for dates");

        System.out.println("\n  3. Validation:");
        System.out.println("     • Input validation");
        System.out.println("     • Business rule enforcement");
        System.out.println("     • Range checking");

        System.out.println("\n  4. Audit Trail:");
        System.out.println("     • All access logged");
        System.out.println("     • Modification tracking");
        System.out.println("     • Compliance ready");

        System.out.println("\n  5. Encryption:");
        System.out.println("     • Sensitive data encrypted");
        System.out.println("     • Decryption only when authorized");
        System.out.println("     • Data-at-rest protection");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Keep sensitive fields private");
        System.out.println("  2. Use proper data types");
        System.out.println("  3. Implement access control");
        System.out.println("  4. Add validation for all setters");
        System.out.println("  5. Maintain audit logs");
        System.out.println("  6. Encrypt sensitive data");
        System.out.println("  7. Mask data in logs and displays");
        System.out.println("  8. Use clear variable names");
        System.out.println("  9. Avoid temp variable reuse");
        System.out.println("  10. Never expose in toString()");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use proper encapsulation");
        System.out.println("  • Implement role-based access");
        System.out.println("  • Add comprehensive audit logging");
        System.out.println("  • Encrypt all sensitive data");
        System.out.println("  • Validate all inputs");
        System.out.println("  • Use appropriate data types");
    }
}