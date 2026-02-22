package deepseek.java;

import java.util.*;
import java.text.*;
import java.time.*;
import java.time.format.*;

public class cwe_374 {

    // ==================== INSECURE IMPLEMENTATION (CWE-374 VULNERABLE)
    // ====================

    static class InsecureUserProfile {
        private String userId;
        private String username;
        private String email;
        private Date birthDate; // Mutable field
        private Date lastLogin; // Mutable field
        private Date accountExpiry; // Mutable field
        private Date passwordChanged; // Mutable field
        private List<String> permissions; // Mutable collection
        private HashMap<String, Object> metadata; // Mutable map

        public InsecureUserProfile(String userId, String username) {
            this.userId = userId;
            this.username = username;
            this.birthDate = new Date();
            this.lastLogin = new Date();
            this.accountExpiry = new Date(System.currentTimeMillis() + 365L * 24 * 60 * 60 * 1000);
            this.passwordChanged = new Date();
            this.permissions = new ArrayList<>();
            this.metadata = new HashMap<>();

            // Add default permissions
            permissions.add("VIEW");
            permissions.add("EDIT");
        }

        // INSECURE: Returns direct reference to mutable Date
        public Date getBirthDate() {
            return birthDate; // Caller can modify this!
        }

        // INSECURE: Returns direct reference to mutable Date
        public Date getLastLogin() {
            return lastLogin; // Caller can modify this!
        }

        // INSECURE: Returns direct reference to mutable Date
        public Date getAccountExpiry() {
            return accountExpiry; // Caller can modify this!
        }

        // INSECURE: Returns direct reference to mutable collection
        public List<String> getPermissions() {
            return permissions; // Caller can modify this!
        }

        // INSECURE: Returns direct reference to mutable map
        public Map<String, Object> getMetadata() {
            return metadata; // Caller can modify this!
        }

        // INSECURE: Setter also accepts mutable objects directly
        public void setBirthDate(Date birthDate) {
            this.birthDate = birthDate; // Stores direct reference to mutable object
        }

        @Override
        public String toString() {
            return String.format("InsecureUser{id='%s', username='%s', birth=%s, login=%s}",
                    userId, username, birthDate, lastLogin);
        }
    }

    // ==================== INSECURE IMPLEMENTATION WITH ARRAY ====================

    static class InsecureDocument {
        private byte[] content; // Mutable array
        private Date created; // Mutable Date
        private Date modified; // Mutable Date
        private String[] tags; // Mutable array

        public InsecureDocument(byte[] content) {
            this.content = content; // Stores direct reference
            this.created = new Date();
            this.modified = new Date();
            this.tags = new String[] { "draft", "private" };
        }

        // INSECURE: Returns direct reference to mutable array
        public byte[] getContent() {
            return content; // Caller can modify the array contents!
        }

        // INSECURE: Returns direct reference to mutable array
        public String[] getTags() {
            return tags; // Caller can modify the array!
        }

        // INSECURE: Returns direct reference to mutable Date
        public Date getCreated() {
            return created; // Caller can modify!
        }
    }

    // ==================== SECURE IMPLEMENTATION (CWE-374 MITIGATED)
    // ====================

    static class SecureUserProfile {
        private String userId;
        private String username;
        private String email;
        private final Date birthDate; // Final reference (still mutable object!)
        private final Date lastLogin; // Final reference (still mutable object!)
        private final Date accountExpiry; // Final reference (still mutable object!)
        private final Date passwordChanged; // Final reference (still mutable object!)
        private final List<String> permissions; // Final reference to mutable collection
        private final Map<String, Object> metadata; // Final reference to mutable map

        // Use LocalDate instead of Date (immutable)
        private LocalDate immutableBirthDate;
        private LocalDateTime immutableLastLogin;

        public SecureUserProfile(String userId, String username) {
            this.userId = userId;
            this.username = username;

            // SECURE: Store defensive copies of mutable objects
            this.birthDate = new Date(); // Will need defensive copying in getters
            this.lastLogin = new Date();
            this.accountExpiry = new Date(System.currentTimeMillis() + 365L * 24 * 60 * 60 * 1000);
            this.passwordChanged = new Date();

            // SECURE: Use immutable alternatives where possible
            this.immutableBirthDate = LocalDate.now();
            this.immutableLastLogin = LocalDateTime.now();

            // SECURE: Wrap collections with unmodifiable views
            List<String> perms = new ArrayList<>();
            perms.add("VIEW");
            perms.add("EDIT");
            this.permissions = Collections.unmodifiableList(perms);

            Map<String, Object> meta = new HashMap<>();
            meta.put("created", LocalDateTime.now().toString());
            this.metadata = Collections.unmodifiableMap(meta);
        }

        // SECURE: Return defensive copy
        public Date getBirthDate() {
            return new Date(birthDate.getTime()); // Defensive copy
        }

        // SECURE: Return defensive copy
        public Date getLastLogin() {
            return new Date(lastLogin.getTime()); // Defensive copy
        }

        // SECURE: Return defensive copy
        public Date getAccountExpiry() {
            return new Date(accountExpiry.getTime()); // Defensive copy
        }

        // SECURE: Return immutable alternative
        public LocalDate getImmutableBirthDate() {
            return immutableBirthDate; // LocalDate is immutable, safe to return directly
        }

        // SECURE: Return immutable alternative
        public LocalDateTime getImmutableLastLogin() {
            return immutableLastLogin; // LocalDateTime is immutable
        }

        // SECURE: Return unmodifiable view
        public List<String> getPermissions() {
            return permissions; // Already unmodifiable
        }

        // SECURE: Return unmodifiable view
        public Map<String, Object> getMetadata() {
            return metadata; // Already unmodifiable
        }

        // SECURE: Setter stores defensive copy
        public void setBirthDate(Date birthDate) {
            // Store defensive copy
            this.birthDate.setTime(birthDate.getTime());
            // Also update immutable version
            this.immutableBirthDate = birthDate.toInstant()
                    .atZone(ZoneId.systemDefault())
                    .toLocalDate();
        }

        // SECURE: Setter using immutable type
        public void setImmutableBirthDate(LocalDate birthDate) {
            this.immutableBirthDate = birthDate; // Immutable, safe
        }

        // Method to demonstrate security breach
        public void demonstrateBreach(InsecureUserProfile profile) {
            System.out.println("\n[SECURE] Attempting to modify insecure profile...");
            Date stolenBirth = profile.getBirthDate();
            stolenBirth.setTime(0); // Modifies original! (1970-01-01)
            System.out.println("  Modified birth date to epoch start");
        }
    }

    // ==================== SECURE DOCUMENT IMPLEMENTATION ====================

    static class SecureDocument {
        private final byte[] content; // Final array - still mutable contents!
        private final Date created;
        private final Date modified;
        private final String[] tags; // Final array - still mutable!

        // Immutable alternatives
        private final Instant createdInstant;
        private final List<String> immutableTags;

        public SecureDocument(byte[] content, String[] tags) {
            // SECURE: Store defensive copies
            this.content = Arrays.copyOf(content, content.length);
            this.created = new Date();
            this.modified = new Date();
            this.tags = Arrays.copyOf(tags, tags.length);

            // Use immutable alternatives
            this.createdInstant = Instant.now();
            this.immutableTags = List.of(tags); // Java 9+ immutable list
        }

        // SECURE: Return defensive copy of array
        public byte[] getContent() {
            return Arrays.copyOf(content, content.length);
        }

        // SECURE: Return defensive copy of array
        public String[] getTags() {
            return Arrays.copyOf(tags, tags.length);
        }

        // SECURE: Return defensive copy of Date
        public Date getCreated() {
            return new Date(created.getTime());
        }

        // SECURE: Return immutable Instant
        public Instant getCreatedInstant() {
            return createdInstant; // Instant is immutable
        }

        // SECURE: Return immutable list
        public List<String> getImmutableTags() {
            return immutableTags; // Already immutable
        }

        // SECURE: Return copy of modified date
        public Date getModified() {
            return new Date(modified.getTime());
        }

        // Method to update content safely
        public void updateContent(byte[] newContent) {
            // Store defensive copy
            System.arraycopy(newContent, 0, this.content, 0,
                    Math.min(newContent.length, this.content.length));
            this.modified.setTime(System.currentTimeMillis());
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class SecurityDemonstrator {

        public static void demonstrateInsecureDate() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE DATE EXPOSURE DEMONSTRATION");
            System.out.println("=".repeat(60));

            InsecureUserProfile profile = new InsecureUserProfile("U001", "john_doe");

            System.out.println("\nOriginal profile:");
            System.out.println("  Birth date: " + profile.getBirthDate());
            System.out.println("  Last login: " + profile.getLastLogin());

            // MALICIOUS CODE: External code modifies the Date
            System.out.println("\n🔴 MALICIOUS ACTOR: Got reference to birth date");
            Date stolenBirthDate = profile.getBirthDate();

            // Modify the original object through the reference
            stolenBirthDate.setYear(100); // Year 2000 (1900+100)
            stolenBirthDate.setMonth(0); // January
            stolenBirthDate.setDate(1); // 1st

            System.out.println("  Modified birth date through reference");

            System.out.println("\nProfile after modification:");
            System.out.println("  Birth date: " + profile.getBirthDate());
            System.out.println("  Last login: " + profile.getLastLogin());

            if (!profile.getBirthDate().equals(new Date())) {
                System.out.println("\n⚠️ SECURITY BREACH: Birth date was modified externally!");
            }
        }

        public static void demonstrateInsecureCollection() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE COLLECTION EXPOSURE DEMONSTRATION");
            System.out.println("=".repeat(60));

            InsecureUserProfile profile = new InsecureUserProfile("U002", "jane_doe");

            System.out.println("\nOriginal permissions: " + profile.getPermissions());

            // MALICIOUS CODE: Modify permissions
            System.out.println("\n🔴 MALICIOUS ACTOR: Got reference to permissions list");
            List<String> stolenPermissions = profile.getPermissions();

            // Add admin permission
            stolenPermissions.add("ADMIN");
            stolenPermissions.remove("VIEW");

            System.out.println("  Modified permissions through reference");

            System.out.println("\nProfile permissions after modification: " +
                    profile.getPermissions());

            if (profile.getPermissions().contains("ADMIN")) {
                System.out.println("\n⚠️ SECURITY BREACH: User gained ADMIN permissions!");
            }
        }

        public static void demonstrateInsecureArray() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE ARRAY EXPOSURE DEMONSTRATION");
            System.out.println("=".repeat(60));

            byte[] sensitiveData = "SecretPassword123".getBytes();
            InsecureDocument doc = new InsecureDocument(sensitiveData);

            System.out.println("\nOriginal content: " +
                    new String(doc.getContent()));

            // MALICIOUS CODE: Modify array
            System.out.println("\n🔴 MALICIOUS ACTOR: Got reference to content array");
            byte[] stolenContent = doc.getContent();

            // Overwrite the data
            Arrays.fill(stolenContent, (byte) 0);
            System.out.println("  Cleared content through reference");

            System.out.println("\nDocument content after modification: " +
                    new String(doc.getContent()));
        }

        public static void demonstrateSecureDate() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE DATE HANDLING DEMONSTRATION");
            System.out.println("=".repeat(60));

            SecureUserProfile profile = new SecureUserProfile("U003", "alice");

            System.out.println("\nOriginal profile:");
            System.out.println("  Birth date (Date): " + profile.getBirthDate());
            System.out.println("  Birth date (LocalDate): " + profile.getImmutableBirthDate());

            // Attempt to modify through reference
            System.out.println("\n🟢 ATTEMPT: Trying to modify through getter");
            Date stolenDate = profile.getBirthDate();

            // Try to modify the copy
            stolenDate.setYear(2000);
            System.out.println("  Modified the copy - year set to 2000");

            System.out.println("\nProfile after modification attempt:");
            System.out.println("  Birth date (Date): " + profile.getBirthDate());
            System.out.println("  Birth date (LocalDate): " + profile.getImmutableBirthDate());

            if (!profile.getBirthDate().equals(stolenDate)) {
                System.out.println("\n✅ PROTECTED: Original date unchanged!");
            }

            // Try with immutable LocalDate
            System.out.println("\n🟢 ATTEMPT: Trying to modify immutable LocalDate");
            LocalDate stolenLocalDate = profile.getImmutableBirthDate();
            // Cannot modify LocalDate - it's immutable!
            // stolenLocalDate = stolenLocalDate.plusDays(1) would create new object

            System.out.println("  Immutable date remains: " + profile.getImmutableBirthDate());
        }

        public static void demonstrateSecureCollection() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE COLLECTION HANDLING DEMONSTRATION");
            System.out.println("=".repeat(60));

            SecureUserProfile profile = new SecureUserProfile("U004", "bob");

            System.out.println("\nOriginal permissions: " + profile.getPermissions());

            // Attempt to modify
            System.out.println("\n🟢 ATTEMPT: Trying to modify permissions");
            List<String> stolenPermissions = profile.getPermissions();

            try {
                stolenPermissions.add("ADMIN");
                System.out.println("  Modified permissions? (should fail)");
            } catch (UnsupportedOperationException e) {
                System.out.println("  ✅ BLOCKED: Cannot modify unmodifiable collection: " +
                        e.getMessage());
            }

            System.out.println("\nPermissions after attempt: " + profile.getPermissions());
        }

        public static void demonstrateSecureArray() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE ARRAY HANDLING DEMONSTRATION");
            System.out.println("=".repeat(60));

            byte[] sensitiveData = "TopSecretData".getBytes();
            String[] tags = { "confidential", "restricted" };
            SecureDocument doc = new SecureDocument(sensitiveData, tags);

            System.out.println("\nOriginal content: " + new String(doc.getContent()));
            System.out.println("Original tags: " + Arrays.toString(doc.getTags()));

            // Attempt to modify through reference
            System.out.println("\n🟢 ATTEMPT: Trying to modify content through getter");
            byte[] stolenContent = doc.getContent();

            // Try to modify the copy
            Arrays.fill(stolenContent, (byte) 0);
            System.out.println("  Modified the copy - cleared content");

            System.out.println("\nDocument after modification attempt:");
            System.out.println("  Content: " + new String(doc.getContent()));
            System.out.println("  Tags: " + Arrays.toString(doc.getTags()));

            if (!Arrays.equals(doc.getContent(), stolenContent)) {
                System.out.println("\n✅ PROTECTED: Original content unchanged!");
            }

            // Try with immutable list
            System.out.println("\n🟢 ATTEMPT: Trying to modify immutable tags");
            List<String> immutableTags = doc.getImmutableTags();

            try {
                immutableTags.add("new-tag");
                System.out.println("  Modified tags? (should fail)");
            } catch (UnsupportedOperationException e) {
                System.out.println("  ✅ BLOCKED: Cannot modify immutable list");
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-374: Passing Mutable Objects to Untrusted Method");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE Date exposure (CWE-374)");
            System.out.println("  2. Demonstrate INSECURE Collection exposure");
            System.out.println("  3. Demonstrate INSECURE Array exposure");
            System.out.println("  4. Demonstrate SECURE Date handling");
            System.out.println("  5. Demonstrate SECURE Collection handling");
            System.out.println("  6. Demonstrate SECURE Array handling");
            System.out.println("  7. Run all demonstrations");
            System.out.println("  8. Show security analysis");
            System.out.println("  9. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    SecurityDemonstrator.demonstrateInsecureDate();
                    break;

                case "2":
                    SecurityDemonstrator.demonstrateInsecureCollection();
                    break;

                case "3":
                    SecurityDemonstrator.demonstrateInsecureArray();
                    break;

                case "4":
                    SecurityDemonstrator.demonstrateSecureDate();
                    break;

                case "5":
                    SecurityDemonstrator.demonstrateSecureCollection();
                    break;

                case "6":
                    SecurityDemonstrator.demonstrateSecureArray();
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

        SecurityDemonstrator.demonstrateInsecureDate();
        SecurityDemonstrator.demonstrateInsecureCollection();
        SecurityDemonstrator.demonstrateInsecureArray();
        SecurityDemonstrator.demonstrateSecureDate();
        SecurityDemonstrator.demonstrateSecureCollection();
        SecurityDemonstrator.demonstrateSecureArray();
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-374");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-374 VULNERABLE):");
        System.out.println("  1. Direct Reference Exposure:");
        System.out.println("     • Returns reference to internal Date objects");
        System.out.println("     • Caller can modify internal state");
        System.out.println("     • No control over modifications");

        System.out.println("\n  2. Mutable Collection Exposure:");
        System.out.println("     • Returns reference to internal List/Map");
        System.out.println("     • Caller can add/remove elements");
        System.out.println("     • Can bypass security checks");

        System.out.println("\n  3. Array Exposure:");
        System.out.println("     • Returns reference to internal array");
        System.out.println("     • Array contents can be modified");
        System.out.println("     • No copy protection");

        System.out.println("\n  4. Consequences:");
        System.out.println("     • Data corruption");
        System.out.println("     • Privilege escalation");
        System.out.println("     • Security bypass");
        System.out.println("     • Inconsistent state");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Defensive Copying:");
        System.out.println("     • Return new Date(oldDate.getTime())");
        System.out.println("     • Return Arrays.copyOf(array)");
        System.out.println("     • Original state protected");

        System.out.println("\n  2. Immutable Objects:");
        System.out.println("     • Use LocalDate instead of Date");
        System.out.println("     • Use Instant instead of Date");
        System.out.println("     • Use List.of() for immutable collections");

        System.out.println("\n  3. Unmodifiable Views:");
        System.out.println("     • Collections.unmodifiableList()");
        System.out.println("     • Collections.unmodifiableMap()");
        System.out.println("     • Collections.unmodifiableSet()");

        System.out.println("\n  4. Defensive Storage:");
        System.out.println("     • Store defensive copies in setters");
        System.out.println("     • Copy arrays in constructors");
        System.out.println("     • Use immutable fields where possible");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Always return defensive copies of mutable objects");
        System.out.println("  2. Prefer immutable objects (LocalDate, Instant)");
        System.out.println("  3. Use unmodifiable collections for exposed data");
        System.out.println("  4. Make defensive copies in constructors");
        System.out.println("  5. Store defensive copies in setters");
        System.out.println("  6. Document mutability behavior");
        System.out.println("  7. Consider using Java 8+ Time API");
        System.out.println("  8. Never expose internal arrays directly");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use immutable types (LocalDate, Instant) instead of Date");
        System.out.println("  • Return Collections.unmodifiableList() for collections");
        System.out.println("  • Always copy arrays before returning");
        System.out.println("  • Make defensive copies in constructors and setters");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  public Date getDate() {");
        System.out.println("      return this.date; // Direct reference!");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // SECURE (with Date)");
        System.out.println("  public Date getDate() {");
        System.out.println("      return new Date(this.date.getTime()); // Defensive copy");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // SECURE (with LocalDate)");
        System.out.println("  public LocalDate getDate() {");
        System.out.println("      return this.localDate; // Immutable - safe!");
        System.out.println("  }");

        System.out.println("\n📊 OBJECT MUTABILITY:");
        System.out.println("  Type         | Mutable | Safe to Return");
        System.out.println("  -------------|---------|---------------");
        System.out.println("  Date         |   ✓     |   ❌ (copy)");
        System.out.println("  Calendar     |   ✓     |   ❌ (copy)");
        System.out.println("  ArrayList    |   ✓     |   ❌ (unmodifiable)");
        System.out.println("  HashMap      |   ✓     |   ❌ (unmodifiable)");
        System.out.println("  Array        |   ✓     |   ❌ (copy)");
        System.out.println("  LocalDate    |   ❌    |   ✓");
        System.out.println("  Instant      |   ❌    |   ✓");
        System.out.println("  String       |   ❌    |   ✓");
        System.out.println("  Integer      |   ❌    |   ✓");

        System.out.println("\n⚠️ NOTE: Even final fields can be problematic if");
        System.out.println("  the object they reference is mutable!");
    }
}