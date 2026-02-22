package deepseek.java;

import java.util.*;
import java.lang.reflect.*;

public class cwe_581 {

    // ==================== DEVICE CLASS ====================

    static class Device {
        private String deviceId;
        private String serialNumber;
        private String model;
        private String manufacturer;
        private int year;
        private double price;
        private Map<String, String> specifications;
        private List<String> tags;
        private Date purchaseDate;
        private boolean isActive;

        public Device(String deviceId, String serialNumber, String model,
                String manufacturer, int year) {
            this.deviceId = deviceId;
            this.serialNumber = serialNumber;
            this.model = model;
            this.manufacturer = manufacturer;
            this.year = year;
            this.specifications = new HashMap<>();
            this.tags = new ArrayList<>();
            this.purchaseDate = new Date();
            this.isActive = true;
        }

        // Getters and setters
        public String getDeviceId() {
            return deviceId;
        }

        public void setDeviceId(String deviceId) {
            this.deviceId = deviceId;
        }

        public String getSerialNumber() {
            return serialNumber;
        }

        public void setSerialNumber(String serialNumber) {
            this.serialNumber = serialNumber;
        }

        public String getModel() {
            return model;
        }

        public void setModel(String model) {
            this.model = model;
        }

        public String getManufacturer() {
            return manufacturer;
        }

        public void setManufacturer(String manufacturer) {
            this.manufacturer = manufacturer;
        }

        public int getYear() {
            return year;
        }

        public void setYear(int year) {
            this.year = year;
        }

        public double getPrice() {
            return price;
        }

        public void setPrice(double price) {
            this.price = price;
        }

        public Map<String, String> getSpecifications() {
            return specifications;
        }

        public void addSpecification(String key, String value) {
            specifications.put(key, value);
        }

        public List<String> getTags() {
            return tags;
        }

        public void addTag(String tag) {
            tags.add(tag);
        }

        public Date getPurchaseDate() {
            return purchaseDate;
        }

        public void setPurchaseDate(Date purchaseDate) {
            this.purchaseDate = purchaseDate;
        }

        public boolean isActive() {
            return isActive;
        }

        public void setActive(boolean active) {
            isActive = active;
        }

        @Override
        public String toString() {
            return String.format("Device{id='%s', sn='%s', model='%s', mfg='%s', year=%d, active=%s}",
                    deviceId, serialNumber, model, manufacturer, year, isActive);
        }
    }

    // ==================== INSECURE DEVICE (CWE-581 VULNERABLE)
    // ====================

    static class InsecureDevice extends Device {

        public InsecureDevice(String deviceId, String serialNumber, String model,
                String manufacturer, int year) {
            super(deviceId, serialNumber, model, manufacturer, year);
        }

        // INSECURE: Override equals but not hashCode
        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;

            InsecureDevice that = (InsecureDevice) obj;

            // Compare by serialNumber only
            return Objects.equals(getSerialNumber(), that.getSerialNumber());
        }

        // INSECURE: No hashCode override!
        // This violates the contract: equal objects must have equal hashCodes
    }

    // ==================== INSECURE DEVICE 2 (WRONG HASHCODE IMPLEMENTATION)
    // ====================

    static class InsecureDevice2 extends Device {

        public InsecureDevice2(String deviceId, String serialNumber, String model,
                String manufacturer, int year) {
            super(deviceId, serialNumber, model, manufacturer, year);
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;

            InsecureDevice2 that = (InsecureDevice2) obj;

            return Objects.equals(getSerialNumber(), that.getSerialNumber()) &&
                    Objects.equals(getModel(), that.getModel()) &&
                    getYear() == that.getYear();
        }

        // INSECURE: Wrong hashCode implementation (constant hash code)
        @Override
        public int hashCode() {
            return 42; // All objects get same hash code - terrible performance!
        }
    }

    // ==================== INSECURE DEVICE 3 (INCONSISTENT EQUALS)
    // ====================

    static class InsecureDevice3 extends Device {

        public InsecureDevice3(String deviceId, String serialNumber, String model,
                String manufacturer, int year) {
            super(deviceId, serialNumber, model, manufacturer, year);
        }

        // INSECURE: Mutable field used in equals
        private boolean isSpecial = false;

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;

            InsecureDevice3 that = (InsecureDevice3) obj;

            // Uses mutable field in equals - dangerous!
            return Objects.equals(getSerialNumber(), that.getSerialNumber()) &&
                    isSpecial == that.isSpecial;
        }

        @Override
        public int hashCode() {
            return Objects.hash(getSerialNumber(), isSpecial);
        }

        public void setSpecial(boolean special) {
            isSpecial = special;
        }
    }

    // ==================== SECURE DEVICE (CWE-581 MITIGATED) ====================

    static class SecureDevice extends Device {

        public SecureDevice(String deviceId, String serialNumber, String model,
                String manufacturer, int year) {
            super(deviceId, serialNumber, model, manufacturer, year);
        }

        // SECURE: Override both equals and hashCode
        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;

            SecureDevice that = (SecureDevice) obj;

            // Use only immutable fields for equality
            return Objects.equals(getSerialNumber(), that.getSerialNumber());
        }

        // SECURE: Consistent hashCode implementation
        @Override
        public int hashCode() {
            // Use same fields as equals()
            return Objects.hashCode(getSerialNumber());
        }
    }

    // ==================== SECURE DEVICE 2 (MULTI-FIELD EQUALS)
    // ====================

    static class SecureDevice2 extends Device {

        public SecureDevice2(String deviceId, String serialNumber, String model,
                String manufacturer, int year) {
            super(deviceId, serialNumber, model, manufacturer, year);
        }

        // SECURE: Multi-field equality
        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null || getClass() != obj.getClass())
                return false;

            SecureDevice2 that = (SecureDevice2) obj;

            // Use multiple immutable fields
            return getYear() == that.getYear() &&
                    Objects.equals(getSerialNumber(), that.getSerialNumber()) &&
                    Objects.equals(getModel(), that.getModel()) &&
                    Objects.equals(getManufacturer(), that.getManufacturer());
        }

        // SECURE: HashCode uses same fields as equals
        @Override
        public int hashCode() {
            return Objects.hash(getSerialNumber(), getModel(), getManufacturer(), getYear());
        }
    }

    // ==================== TEST UTILITIES ====================

    static class EqualityTester {

        public static void testEquality(String testName, Object obj1, Object obj2) {
            System.out.println("\n" + "-".repeat(50));
            System.out.println("TEST: " + testName);
            System.out.println("-".repeat(50));

            System.out.println("Object 1: " + obj1);
            System.out.println("Object 2: " + obj2);

            boolean equals = obj1.equals(obj2);
            System.out.println("obj1.equals(obj2): " + equals);

            if (equals) {
                int hash1 = obj1.hashCode();
                int hash2 = obj2.hashCode();
                System.out.println("obj1.hashCode(): " + hash1);
                System.out.println("obj2.hashCode(): " + hash2);
                System.out.println("Hash codes equal: " + (hash1 == hash2));

                // Contract violation detection
                if (hash1 != hash2) {
                    System.out.println("⚠️ CONTRACT VIOLATION: Equal objects must have equal hashCodes!");
                }
            }
        }

        public static void testCollectionBehavior(String testName, Object obj1, Object obj2) {
            System.out.println("\n" + "-".repeat(50));
            System.out.println("COLLECTION TEST: " + testName);
            System.out.println("-".repeat(50));

            // Test HashSet behavior
            Set<Object> hashSet = new HashSet<>();
            hashSet.add(obj1);

            System.out.println("HashSet contains obj1: " + hashSet.contains(obj1));
            System.out.println("HashSet contains obj2: " + hashSet.contains(obj2));
            System.out.println("HashSet size: " + hashSet.size());

            // Test HashMap behavior
            Map<Object, String> hashMap = new HashMap<>();
            hashMap.put(obj1, "Value for obj1");

            System.out.println("\nHashMap get(obj1): " + hashMap.get(obj1));
            System.out.println("HashMap get(obj2): " + hashMap.get(obj2));

            // Test TreeSet (relies on Comparable or Comparator, not hashCode)
            try {
                Set<Object> treeSet = new TreeSet<>((a, b) -> {
                    if (a.equals(b))
                        return 0;
                    return System.identityHashCode(a) - System.identityHashCode(b);
                });
                treeSet.add(obj1);
                System.out.println("\nTreeSet contains obj1: " + treeSet.contains(obj1));
                System.out.println("TreeSet contains obj2: " + treeSet.contains(obj2));
            } catch (Exception e) {
                System.out.println("\nTreeSet test failed: " + e.getMessage());
            }
        }

        public static void testPerformance(String testName, List<?> objects) {
            System.out.println("\n" + "-".repeat(50));
            System.out.println("PERFORMANCE TEST: " + testName);
            System.out.println("-".repeat(50));

            long startTime = System.nanoTime();
            Set<Object> set = new HashSet<>(objects);
            long endTime = System.nanoTime();

            System.out.println("Set creation time: " + (endTime - startTime) / 1_000_000.0 + " ms");
            System.out.println("Set size: " + set.size() + " (objects: " + objects.size() + ")");

            if (set.size() < objects.size()) {
                System.out.println("⚠️ Duplicates detected due to hashCode/equals issues!");
            }
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class ContractDemonstrator {

        public static void demonstrateEqualsContract() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("EQUALS CONTRACT DEMONSTRATION");
            System.out.println("=".repeat(60));

            SecureDevice d1 = new SecureDevice("D001", "SN12345", "Laptop", "Dell", 2023);
            SecureDevice d2 = new SecureDevice("D001", "SN12345", "Laptop", "Dell", 2023);
            SecureDevice d3 = new SecureDevice("D002", "SN12345", "Laptop", "Dell", 2023);

            System.out.println("\n1. Reflexive: x.equals(x) must be true");
            System.out.println("   d1.equals(d1): " + d1.equals(d1) + " ✓");

            System.out.println("\n2. Symmetric: x.equals(y) == y.equals(x)");
            System.out.println("   d1.equals(d2): " + d1.equals(d2));
            System.out.println("   d2.equals(d1): " + d2.equals(d1));
            System.out.println("   Symmetric: " + (d1.equals(d2) == d2.equals(d1)) + " ✓");

            System.out.println("\n3. Transitive: if x.equals(y) and y.equals(z), then x.equals(z)");
            System.out.println("   d1.equals(d2): " + d1.equals(d2));
            System.out.println("   d2.equals(d3): " + d2.equals(d3));
            System.out.println("   d1.equals(d3): " + d1.equals(d3));

            System.out.println("\n4. Consistent: multiple invocations return same result");
            System.out.println("   d1.equals(d2) (1st): " + d1.equals(d2));
            System.out.println("   d1.equals(d2) (2nd): " + d1.equals(d2));

            System.out.println("\n5. Non-null: x.equals(null) must be false");
            System.out.println("   d1.equals(null): " + d1.equals(null) + " ✓");
        }

        public static void demonstrateHashCodeContract() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("HASHCODE CONTRACT DEMONSTRATION");
            System.out.println("=".repeat(60));

            SecureDevice d1 = new SecureDevice("D001", "SN12345", "Laptop", "Dell", 2023);
            SecureDevice d2 = new SecureDevice("D001", "SN12345", "Laptop", "Dell", 2023);
            SecureDevice d3 = new SecureDevice("D002", "SN67890", "Desktop", "HP", 2022);

            System.out.println("\n1. Consistent: same object returns same hashCode");
            System.out.println("   d1.hashCode() (1st): " + d1.hashCode());
            System.out.println("   d1.hashCode() (2nd): " + d1.hashCode());

            System.out.println("\n2. Equal objects must have equal hashCodes");
            System.out.println("   d1.equals(d2): " + d1.equals(d2));
            System.out.println("   d1.hashCode(): " + d1.hashCode());
            System.out.println("   d2.hashCode(): " + d2.hashCode());
            System.out.println("   Equal hashCodes: " + (d1.hashCode() == d2.hashCode()) + " ✓");

            System.out.println("\n3. Unequal objects may have same hashCode (collision)");
            System.out.println("   d1.equals(d3): " + d1.equals(d3));
            System.out.println("   d1.hashCode(): " + d1.hashCode());
            System.out.println("   d3.hashCode(): " + d3.hashCode());
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-581: Object Model Violation - Equals without HashCode");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE Device (equals only) - CWE-581");
            System.out.println("  2. Demonstrate INSECURE Device2 (bad hashCode)");
            System.out.println("  3. Demonstrate INSECURE Device3 (mutable equals)");
            System.out.println("  4. Demonstrate SECURE Device (equals + hashCode)");
            System.out.println("  5. Demonstrate SECURE Device2 (multi-field)");
            System.out.println("  6. Compare all implementations");
            System.out.println("  7. Show equals/hashCode contract");
            System.out.println("  8. Test collection behavior");
            System.out.println("  9. Show security analysis");
            System.out.println("  10. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecureDevice();
                    break;

                case "2":
                    demonstrateInsecureDevice2();
                    break;

                case "3":
                    demonstrateInsecureDevice3();
                    break;

                case "4":
                    demonstrateSecureDevice();
                    break;

                case "5":
                    demonstrateSecureDevice2();
                    break;

                case "6":
                    compareImplementations();
                    break;

                case "7":
                    demonstrateContracts();
                    break;

                case "8":
                    testCollectionBehavior(scanner);
                    break;

                case "9":
                    showSecurityAnalysis();
                    break;

                case "10":
                    System.out.println("\nExiting...");
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void demonstrateInsecureDevice() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE DEVICE (equals only, no hashCode)");
        System.out.println("=".repeat(50));

        InsecureDevice d1 = new InsecureDevice("D001", "SN12345", "Laptop", "Dell", 2023);
        InsecureDevice d2 = new InsecureDevice("D002", "SN12345", "Laptop", "Dell", 2023); // Same SN
        InsecureDevice d3 = new InsecureDevice("D003", "SN67890", "Desktop", "HP", 2022);

        EqualityTester.testEquality("Same serial number devices", d1, d2);
        EqualityTester.testCollectionBehavior("InsecureDevice in HashSet", d1, d2);

        System.out.println("\n⚠️ PROBLEM: Even though d1 and d2 are equal (same SN),");
        System.out.println("   HashSet treats them as different because hashCodes differ!");
    }

    private static void demonstrateInsecureDevice2() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE DEVICE2 (constant hashCode)");
        System.out.println("=".repeat(50));

        InsecureDevice2 d1 = new InsecureDevice2("D001", "SN12345", "Laptop", "Dell", 2023);
        InsecureDevice2 d2 = new InsecureDevice2("D002", "SN12345", "Laptop", "Dell", 2023);
        InsecureDevice2 d3 = new InsecureDevice2("D003", "SN67890", "Desktop", "HP", 2022);

        EqualityTester.testEquality("Same serial number devices", d1, d2);
        EqualityTester.testCollectionBehavior("InsecureDevice2 in HashSet", d1, d2);

        System.out.println("\n⚠️ PROBLEM: All objects have same hashCode (42)!");
        System.out.println("   This causes terrible HashSet performance (all in same bucket).");

        // Performance test
        List<InsecureDevice2> devices = new ArrayList<>();
        for (int i = 0; i < 1000; i++) {
            devices.add(new InsecureDevice2("D" + i, "SN" + i, "Model", "Mfg", 2023));
        }
        EqualityTester.testPerformance("InsecureDevice2 (constant hash)", devices);
    }

    private static void demonstrateInsecureDevice3() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE DEVICE3 (mutable field in equals)");
        System.out.println("=".repeat(50));

        InsecureDevice3 d1 = new InsecureDevice3("D001", "SN12345", "Laptop", "Dell", 2023);
        InsecureDevice3 d2 = new InsecureDevice3("D002", "SN12345", "Laptop", "Dell", 2023);

        System.out.println("\nInitial state:");
        System.out.println("d1 special: false, d2 special: false");
        EqualityTester.testCollectionBehavior("Before mutation", d1, d2);

        System.out.println("\nAfter modifying d1:");
        d1.setSpecial(true);
        System.out.println("d1 special: true, d2 special: false");

        // HashSet lookup fails because hash code changed
        Set<Object> set = new HashSet<>();
        set.add(d1);
        System.out.println("HashSet contains d1: " + set.contains(d1));
        System.out.println("HashSet contains d2: " + set.contains(d2));

        System.out.println("\n⚠️ PROBLEM: Object's hashCode changed after being added to HashSet!");
        System.out.println("   This causes the object to be 'lost' in the collection.");
    }

    private static void demonstrateSecureDevice() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SECURE DEVICE (proper equals + hashCode)");
        System.out.println("=".repeat(50));

        SecureDevice d1 = new SecureDevice("D001", "SN12345", "Laptop", "Dell", 2023);
        SecureDevice d2 = new SecureDevice("D002", "SN12345", "Laptop", "Dell", 2023);
        SecureDevice d3 = new SecureDevice("D003", "SN67890", "Desktop", "HP", 2022);

        EqualityTester.testEquality("Same serial number devices", d1, d2);
        EqualityTester.testCollectionBehavior("SecureDevice in HashSet", d1, d2);

        System.out.println("\n✅ CORRECT: Equal objects have equal hashCodes,");
        System.out.println("   HashSet correctly identifies duplicates.");
    }

    private static void demonstrateSecureDevice2() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SECURE DEVICE2 (multi-field equals + hashCode)");
        System.out.println("=".repeat(50));

        SecureDevice2 d1 = new SecureDevice2("D001", "SN12345", "Laptop", "Dell", 2023);
        SecureDevice2 d2 = new SecureDevice2("D002", "SN12345", "Laptop", "Dell", 2023);
        SecureDevice2 d3 = new SecureDevice2("D003", "SN67890", "Desktop", "HP", 2022);

        EqualityTester.testEquality("Same serial number devices", d1, d2);
        EqualityTester.testCollectionBehavior("SecureDevice2 in HashSet", d1, d2);

        System.out.println("\n✅ CORRECT: Multi-field equality with proper hashCode");
    }

    private static void compareImplementations() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("IMPLEMENTATION COMPARISON");
        System.out.println("=".repeat(60));

        // Create objects with same serial number
        InsecureDevice i1 = new InsecureDevice("D001", "SN12345", "Laptop", "Dell", 2023);
        InsecureDevice i2 = new InsecureDevice("D002", "SN12345", "Laptop", "Dell", 2023);

        InsecureDevice2 i2_1 = new InsecureDevice2("D001", "SN12345", "Laptop", "Dell", 2023);
        InsecureDevice2 i2_2 = new InsecureDevice2("D002", "SN12345", "Laptop", "Dell", 2023);

        SecureDevice s1 = new SecureDevice("D001", "SN12345", "Laptop", "Dell", 2023);
        SecureDevice s2 = new SecureDevice("D002", "SN12345", "Laptop", "Dell", 2023);

        System.out.println("\n📊 EQUALS COMPARISON (same serial number):");
        System.out.println("  InsecureDevice:  " + i1.equals(i2) + " (correct)");
        System.out.println("  InsecureDevice2: " + i2_1.equals(i2_2) + " (correct)");
        System.out.println("  SecureDevice:    " + s1.equals(s2) + " (correct)");

        System.out.println("\n📊 HASHCODE COMPARISON (same serial number):");
        System.out.println("  InsecureDevice:  " + i1.hashCode() + " vs " + i2.hashCode() +
                " -> " + (i1.hashCode() == i2.hashCode() ? "EQUAL" : "DIFFERENT ❌"));
        System.out.println("  InsecureDevice2: " + i2_1.hashCode() + " vs " + i2_2.hashCode() +
                " -> " + (i2_1.hashCode() == i2_2.hashCode() ? "EQUAL ✓" : "DIFFERENT ❌"));
        System.out.println("  SecureDevice:    " + s1.hashCode() + " vs " + s2.hashCode() +
                " -> " + (s1.hashCode() == s2.hashCode() ? "EQUAL ✓" : "DIFFERENT ❌"));

        System.out.println("\n📊 HASH SET BEHAVIOR (add both, check size):");

        Set<InsecureDevice> set1 = new HashSet<>();
        set1.add(i1);
        set1.add(i2);
        System.out.println("  InsecureDevice HashSet size: " + set1.size() +
                " (should be 1, but got " + set1.size() + " ❌)");

        Set<InsecureDevice2> set2 = new HashSet<>();
        set2.add(i2_1);
        set2.add(i2_2);
        System.out.println("  InsecureDevice2 HashSet size: " + set2.size() +
                " (should be 1, but got " + set2.size() + " ❌)");

        Set<SecureDevice> set3 = new HashSet<>();
        set3.add(s1);
        set3.add(s2);
        System.out.println("  SecureDevice HashSet size: " + set3.size() +
                " (should be 1, got " + set3.size() + " ✓)");
    }

    private static void demonstrateContracts() {
        ContractDemonstrator.demonstrateEqualsContract();
        ContractDemonstrator.demonstrateHashCodeContract();
    }

    private static void testCollectionBehavior(Scanner scanner) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("COLLECTION BEHAVIOR TEST");
        System.out.println("=".repeat(50));

        System.out.print("\nEnter serial number for test: ");
        String sn = scanner.nextLine().trim();

        InsecureDevice i1 = new InsecureDevice("D001", sn, "Laptop", "Dell", 2023);
        InsecureDevice i2 = new InsecureDevice("D002", sn, "Laptop", "Dell", 2023);

        SecureDevice s1 = new SecureDevice("D001", sn, "Laptop", "Dell", 2023);
        SecureDevice s2 = new SecureDevice("D002", sn, "Laptop", "Dell", 2023);

        System.out.println("\n🔴 INSECURE Device in Collections:");
        EqualityTester.testCollectionBehavior("Insecure test", i1, i2);

        System.out.println("\n✅ SECURE Device in Collections:");
        EqualityTester.testCollectionBehavior("Secure test", s1, s2);
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-581");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATIONS:");

        System.out.println("\n  1. equals() without hashCode():");
        System.out.println("     • Violates Object contract");
        System.out.println("     • Causes incorrect collection behavior");
        System.out.println("     • Equal objects may not be found in HashSets/HashMaps");
        System.out.println("     • Can lead to data corruption");

        System.out.println("\n  2. Bad hashCode() implementation:");
        System.out.println("     • Constant hashCode degrades performance");
        System.out.println("     • All objects in same bucket (O(n) lookup)");
        System.out.println("     • Can cause denial of service");

        System.out.println("\n  3. Mutable fields in equals/hashCode:");
        System.out.println("     • Objects become 'lost' in collections");
        System.out.println("     • Inconsistent behavior");
        System.out.println("     • Security bypass possible");

        System.out.println("\n✅ SECURE IMPLEMENTATION:");
        System.out.println("  1. Always override equals AND hashCode together");
        System.out.println("  2. Use same fields in both methods");
        System.out.println("  3. Use immutable fields only");
        System.out.println("  4. Follow the contracts:");
        System.out.println("     • Reflexive: x.equals(x) true");
        System.out.println("     • Symmetric: x.equals(y) == y.equals(x)");
        System.out.println("     • Transitive: if x.equals(y) and y.equals(z), then x.equals(z)");
        System.out.println("     • Consistent: multiple calls return same result");
        System.out.println("     • Non-null: x.equals(null) false");
        System.out.println("     • Equal objects must have equal hashCodes");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Use @Override annotation to prevent mistakes");
        System.out.println("  2. Use Objects.equals() and Objects.hashCode()");
        System.out.println("  3. Never use mutable fields in equals/hashCode");
        System.out.println("  4. Consider using IDE-generated implementations");
        System.out.println("  5. Test with collections (HashSet, HashMap)");
        System.out.println("  6. Document equality criteria");
        System.out.println("  7. Be careful with inheritance hierarchies");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Always override both equals() and hashCode()");
        System.out.println("  • Use java.util.Objects helper methods");
        System.out.println("  • Keep fields used immutable");
        System.out.println("  • Test thoroughly with collections");

        System.out.println("\n📊 COMMON MISTAKES:");
        System.out.println("  • Forgetting hashCode() when overriding equals()");
        System.out.println("  • Using mutable fields in hashCode()");
        System.out.println("  • Inconsistent field sets in equals() and hashCode()");
        System.out.println("  • Not handling null properly");
        System.out.println("  • Using wrong field types");

        System.out.println("\n✓ CORRECT IMPLEMENTATION:");
        System.out.println("  @Override");
        System.out.println("  public boolean equals(Object obj) {");
        System.out.println("      if (this == obj) return true;");
        System.out.println("      if (obj == null || getClass() != obj.getClass()) return false;");
        System.out.println("      MyClass that = (MyClass) obj;");
        System.out.println("      return Objects.equals(field1, that.field1) &&");
        System.out.println("             Objects.equals(field2, that.field2);");
        System.out.println("  }");
        System.out.println("  ");
        System.out.println("  @Override");
        System.out.println("  public int hashCode() {");
        System.out.println("      return Objects.hash(field1, field2);");
        System.out.println("  }");
    }
}