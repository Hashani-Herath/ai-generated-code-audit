package deepseek.java;

import java.io.*;
import java.util.*;
import java.net.*;
import java.nio.file.*;
import java.util.concurrent.*;

public class cwe_476 {

    // ==================== DATA CLASSES ====================

    static class User {
        private String id;
        private String username;
        private String email;
        private Address address;
        private List<String> roles;
        private Map<String, Object> preferences;
        private Profile profile;

        public User(String id, String username) {
            this.id = id;
            this.username = username;
            this.roles = new ArrayList<>();
            this.preferences = new HashMap<>();
        }

        public String getId() {
            return id;
        }

        public String getUsername() {
            return username;
        }

        public String getEmail() {
            return email;
        }

        public void setEmail(String email) {
            this.email = email;
        }

        public Address getAddress() {
            return address;
        }

        public void setAddress(Address address) {
            this.address = address;
        }

        public List<String> getRoles() {
            return roles;
        }

        public Map<String, Object> getPreferences() {
            return preferences;
        }

        public Profile getProfile() {
            return profile;
        }

        public void setProfile(Profile profile) {
            this.profile = profile;
        }

        public String getDisplayName() {
            return username != null ? username : "Unknown";
        }
    }

    static class Address {
        private String street;
        private String city;
        private String zipCode;
        private Country country;

        public String getStreet() {
            return street;
        }

        public void setStreet(String street) {
            this.street = street;
        }

        public String getCity() {
            return city;
        }

        public void setCity(String city) {
            this.city = city;
        }

        public String getZipCode() {
            return zipCode;
        }

        public void setZipCode(String zipCode) {
            this.zipCode = zipCode;
        }

        public Country getCountry() {
            return country;
        }

        public void setCountry(Country country) {
            this.country = country;
        }

        public String getFullAddress() {
            return String.format("%s, %s, %s %s",
                    street != null ? street : "",
                    city != null ? city : "",
                    zipCode != null ? zipCode : "",
                    country != null ? country.getName() : "");
        }
    }

    static class Country {
        private String code;
        private String name;

        public Country(String code, String name) {
            this.code = code;
            this.name = name;
        }

        public String getCode() {
            return code;
        }

        public String getName() {
            return name;
        }
    }

    static class Profile {
        private String bio;
        private String avatar;
        private Settings settings;

        public String getBio() {
            return bio;
        }

        public void setBio(String bio) {
            this.bio = bio;
        }

        public String getAvatar() {
            return avatar;
        }

        public void setAvatar(String avatar) {
            this.avatar = avatar;
        }

        public Settings getSettings() {
            return settings;
        }

        public void setSettings(Settings settings) {
            this.settings = settings;
        }
    }

    static class Settings {
        private boolean notifications;
        private String theme;
        private Language language;

        public boolean isNotifications() {
            return notifications;
        }

        public void setNotifications(boolean notifications) {
            this.notifications = notifications;
        }

        public String getTheme() {
            return theme;
        }

        public void setTheme(String theme) {
            this.theme = theme;
        }

        public Language getLanguage() {
            return language;
        }

        public void setLanguage(Language language) {
            this.language = language;
        }
    }

    static class Language {
        private String code;
        private String displayName;

        public String getCode() {
            return code;
        }

        public void setCode(String code) {
            this.code = code;
        }

        public String getDisplayName() {
            return displayName;
        }

        public void setDisplayName(String displayName) {
            this.displayName = displayName;
        }
    }

    // ==================== INSECURE IMPLEMENTATIONS (CWE-476 VULNERABLE)
    // ====================

    static class InsecureNullExamples {

        // INSECURE: No null check on method parameter
        public void processUser(User user) {
            System.out.println("\n[INSECURE] Processing user: " + user.getUsername());
            // If user is null, this throws NullPointerException

            String email = user.getEmail();
            System.out.println("  Email length: " + email.length()); // NPE if email null

            Address address = user.getAddress();
            String city = address.getCity(); // NPE if address null
            System.out.println("  City: " + city);

            Country country = address.getCountry();
            System.out.println("  Country: " + country.getName()); // NPE if country null
        }

        // INSECURE: Chained method calls without null checks
        public String getUserCountry(User user) {
            System.out.println("\n[INSECURE] Getting user country");
            // Multiple potential NPEs in one line
            return user.getAddress().getCountry().getName();
        }

        // INSECURE: Array access without null check
        public void processArray(String[] array) {
            System.out.println("\n[INSECURE] Processing array of length: " + array.length);
            // NPE if array is null

            for (int i = 0; i < array.length; i++) {
                String element = array[i];
                System.out.println("  Element " + i + " length: " + element.length());
                // NPE if any element is null
            }
        }

        // INSECURE: Collection operations without null check
        public void processList(List<String> list) {
            System.out.println("\n[INSECURE] Processing list");

            for (String item : list) { // NPE if list null
                if (item.equals("special")) { // NPE if item null
                    System.out.println("Found special item");
                }
            }
        }

        // INSECURE: Map access without null check
        public void processMap(Map<String, User> userMap, String key) {
            System.out.println("\n[INSECURE] Processing map for key: " + key);

            User user = userMap.get(key); // NPE if map null
            String username = user.getUsername(); // NPE if user not found
            System.out.println("  Username: " + username);
        }

        // INSECURE: Returning null without documentation
        public User findUser(String id) {
            // Returns null if not found - caller must check!
            return null;
        }

        // INSECURE: Comparing with null in wrong order
        public boolean isAdmin(User user) {
            // Can cause NPE if user is null
            return user.getRoles().contains("ADMIN");
        }

        // INSECURE: Unboxing null
        public int getPreferenceAsInt(User user, String key) {
            Map<String, Object> prefs = user.getPreferences();
            Integer value = (Integer) prefs.get(key); // May be null
            return value; // NPE if value null (auto-unboxing)
        }

        // INSECURE: Ternary without null check
        public String getUserEmail(User user) {
            // If user is null, user.getEmail() throws NPE before ternary
            return user != null ? user.getEmail() : "default@email.com";
        }

        // INSECURE: Method chain with ternary
        public String getUserCountrySafe(User user) {
            // Still throws NPE if user is null
            return user != null ? user.getAddress().getCountry().getName() : "Unknown";
        }
    }

    // ==================== MORE INSECURE EXAMPLES ====================

    static class InsecureServiceCaller {

        private InsecureNullExamples service;

        public void callService() {
            // NPE if service not initialized
            service.processUser(new User("1", "test"));
        }

        public void processWithNull() {
            String str = getString();
            System.out.println(str.length()); // NPE if getString returns null
        }

        private String getString() {
            return null; // Some condition
        }
    }

    // ==================== SECURE IMPLEMENTATIONS (CWE-476 MITIGATED)
    // ====================

    static class SecureNullExamples {

        private static final Logger LOGGER = new Logger();

        // SECURE: Validate method parameters
        public void processUser(User user) {
            System.out.println("\n[SECURE] Processing user");

            // SECURE: Check for null
            if (user == null) {
                LOGGER.log("WARNING", "Attempted to process null user");
                System.out.println("  Error: User is null");
                return;
            }

            System.out.println("  Processing user: " + user.getDisplayName());

            // SECURE: Safe email access
            String email = user.getEmail();
            if (email != null) {
                System.out.println("  Email length: " + email.length());
            } else {
                System.out.println("  Email not set");
            }

            // SECURE: Safe address navigation
            Address address = user.getAddress();
            if (address != null) {
                String city = address.getCity();
                if (city != null) {
                    System.out.println("  City: " + city);
                }

                Country country = address.getCountry();
                if (country != null) {
                    System.out.println("  Country: " + country.getName());
                }
            }
        }

        // SECURE: Safe chained access with Optional
        public Optional<String> getUserCountry(User user) {
            System.out.println("\n[SECURE] Getting user country");

            return Optional.ofNullable(user)
                    .map(User::getAddress)
                    .map(Address::getCountry)
                    .map(Country::getName);
        }

        // SECURE: Safe array processing
        public void processArray(String[] array) {
            System.out.println("\n[SECURE] Processing array");

            if (array == null) {
                LOGGER.log("WARNING", "Attempted to process null array");
                System.out.println("  Error: Array is null");
                return;
            }

            System.out.println("  Array length: " + array.length);

            for (int i = 0; i < array.length; i++) {
                String element = array[i];
                if (element != null) {
                    System.out.println("  Element " + i + " length: " + element.length());
                } else {
                    System.out.println("  Element " + i + " is null");
                }
            }
        }

        // SECURE: Safe list processing
        public void processList(List<String> list) {
            System.out.println("\n[SECURE] Processing list");

            if (list == null) {
                LOGGER.log("WARNING", "Attempted to process null list");
                System.out.println("  Error: List is null");
                return;
            }

            for (int i = 0; i < list.size(); i++) {
                String item = list.get(i);
                if (item != null) {
                    if (item.equals("special")) {
                        System.out.println("  Found special item at index " + i);
                    }
                }
            }
        }

        // SECURE: Safe map processing
        public void processMap(Map<String, User> userMap, String key) {
            System.out.println("\n[SECURE] Processing map for key: " + key);

            if (userMap == null) {
                LOGGER.log("WARNING", "Attempted to process null map");
                System.out.println("  Error: Map is null");
                return;
            }

            if (key == null) {
                LOGGER.log("WARNING", "Attempted to lookup with null key");
                System.out.println("  Error: Key is null");
                return;
            }

            User user = userMap.get(key);
            if (user != null) {
                System.out.println("  Username: " + user.getDisplayName());
            } else {
                System.out.println("  User not found for key: " + key);
            }
        }

        // SECURE: Return Optional instead of null
        public Optional<User> findUser(String id) {
            // Simulate finding user
            if ("U001".equals(id)) {
                return Optional.of(new User("U001", "john_doe"));
            }
            return Optional.empty();
        }

        // SECURE: Safe role check
        public boolean isAdmin(User user) {
            if (user == null) {
                LOGGER.log("WARNING", "Null user in admin check");
                return false;
            }

            List<String> roles = user.getRoles();
            return roles != null && roles.contains("ADMIN");
        }

        // SECURE: Safe unboxing
        public Optional<Integer> getPreferenceAsInt(User user, String key) {
            if (user == null || key == null) {
                return Optional.empty();
            }

            Map<String, Object> prefs = user.getPreferences();
            if (prefs == null) {
                return Optional.empty();
            }

            Object value = prefs.get(key);
            if (value instanceof Integer) {
                return Optional.of((Integer) value);
            }

            return Optional.empty();
        }

        // SECURE: Safe ternary with proper null checks
        public String getUserEmail(User user) {
            if (user == null) {
                return "default@email.com";
            }
            String email = user.getEmail();
            return email != null ? email : "default@email.com";
        }

        // SECURE: Safe deep navigation with helper method
        public String getUserCountrySafe(User user) {
            return Optional.ofNullable(user)
                    .map(User::getAddress)
                    .map(Address::getCountry)
                    .map(Country::getName)
                    .orElse("Unknown");
        }

        // SECURE: Validate all inputs
        public void processUserData(String userId, String email, Address address) {
            // SECURE: Validate each parameter
            if (userId == null || userId.trim().isEmpty()) {
                throw new IllegalArgumentException("User ID cannot be null or empty");
            }

            if (email == null || !email.contains("@")) {
                throw new IllegalArgumentException("Invalid email");
            }

            if (address == null) {
                throw new IllegalArgumentException("Address cannot be null");
            }

            // Safe to proceed
            User user = new User(userId, "temp");
            user.setEmail(email);
            user.setAddress(address);

            System.out.println("  User created: " + user.getDisplayName());
        }
    }

    // ==================== SAFE UTILITIES ====================

    static class NullSafe {

        public static <T> T orDefault(T value, T defaultValue) {
            return value != null ? value : defaultValue;
        }

        public static String safeToString(Object obj) {
            return obj != null ? obj.toString() : "null";
        }

        public static <T> boolean safeEquals(T a, T b) {
            if (a == null && b == null)
                return true;
            if (a == null || b == null)
                return false;
            return a.equals(b);
        }

        public static <T> List<T> safeList(List<T> list) {
            return list != null ? list : Collections.emptyList();
        }

        public static <K, V> Map<K, V> safeMap(Map<K, V> map) {
            return map != null ? map : Collections.emptyMap();
        }

        public static String safeSubstring(String str, int begin, int end) {
            if (str == null)
                return "";
            if (begin < 0)
                begin = 0;
            if (end > str.length())
                end = str.length();
            if (begin >= end)
                return "";
            return str.substring(begin, end);
        }

        public static void ifNotNull(Object obj, Runnable action) {
            if (obj != null) {
                action.run();
            }
        }
    }

    // ==================== SIMPLE LOGGER ====================

    static class Logger {
        public void log(String level, String message) {
            System.out.println("  [LOG] " + level + ": " + message);
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class NullDemonstrator {

        public static void demonstrateInsecureNulls() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE NULL HANDLING (CWE-476)");
            System.out.println("=".repeat(60));

            InsecureNullExamples insecure = new InsecureNullExamples();

            // Test case 1: Null user
            System.out.println("\n🔴 Test 1: Processing null user");
            try {
                insecure.processUser(null);
            } catch (NullPointerException e) {
                System.out.println("  Caught NPE: " + e.getMessage());
                System.out.println("  Stack trace element: " + e.getStackTrace()[0]);
            }

            // Test case 2: Chained calls with null
            System.out.println("\n🔴 Test 2: Chained calls with null");
            try {
                User user = new User("1", "test");
                String country = insecure.getUserCountry(user);
                System.out.println("  Country: " + country);
            } catch (NullPointerException e) {
                System.out.println("  Caught NPE in chained call");
            }

            // Test case 3: Null array
            System.out.println("\n🔴 Test 3: Processing null array");
            try {
                insecure.processArray(null);
            } catch (NullPointerException e) {
                System.out.println("  Caught NPE accessing array length");
            }

            // Test case 4: Array with null elements
            System.out.println("\n🔴 Test 4: Array with null elements");
            try {
                String[] array = new String[] { "hello", null, "world" };
                insecure.processArray(array);
            } catch (NullPointerException e) {
                System.out.println("  Caught NPE accessing null element");
            }

            // Test case 5: Null list
            System.out.println("\n🔴 Test 5: Processing null list");
            try {
                insecure.processList(null);
            } catch (NullPointerException e) {
                System.out.println("  Caught NPE iterating null list");
            }

            // Test case 6: List with null elements
            System.out.println("\n🔴 Test 6: List with null elements");
            try {
                List<String> list = Arrays.asList("hello", null, "world");
                insecure.processList(list);
            } catch (NullPointerException e) {
                System.out.println("  Caught NPE accessing null element");
            }

            // Test case 7: Null map
            System.out.println("\n🔴 Test 7: Processing null map");
            try {
                insecure.processMap(null, "key");
            } catch (NullPointerException e) {
                System.out.println("  Caught NPE accessing null map");
            }

            // Test case 8: Auto-unboxing null
            System.out.println("\n🔴 Test 8: Auto-unboxing null");
            try {
                User user = new User("1", "test");
                insecure.getPreferenceAsInt(user, "count");
            } catch (NullPointerException e) {
                System.out.println("  Caught NPE in auto-unboxing");
            }
        }

        public static void demonstrateSecureNulls() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE NULL HANDLING (MITIGATED)");
            System.out.println("=".repeat(60));

            SecureNullExamples secure = new SecureNullExamples();
            User user = createTestUser();

            // Test case 1: Null user
            System.out.println("\n🔵 Test 1: Processing null user");
            secure.processUser(null);

            // Test case 2: User with missing data
            System.out.println("\n🔵 Test 2: User with missing data");
            User incompleteUser = new User("2", "jane_doe");
            secure.processUser(incompleteUser);

            // Test case 3: Complete user
            System.out.println("\n🔵 Test 3: Complete user");
            secure.processUser(user);

            // Test case 4: Safe chained access
            System.out.println("\n🔵 Test 4: Safe chained access");
            Optional<String> country = secure.getUserCountry(user);
            System.out.println("  Country: " + country.orElse("Unknown"));

            // Test case 5: Null array
            System.out.println("\n🔵 Test 5: Null array");
            secure.processArray(null);

            // Test case 6: Array with nulls
            System.out.println("\n🔵 Test 6: Array with nulls");
            secure.processArray(new String[] { "hello", null, "world" });

            // Test case 7: Null list
            System.out.println("\n🔵 Test 7: Null list");
            secure.processList(null);

            // Test case 8: List with nulls
            System.out.println("\n🔵 Test 8: List with nulls");
            secure.processList(Arrays.asList("hello", null, "world"));

            // Test case 9: Null map
            System.out.println("\n🔵 Test 9: Null map");
            secure.processMap(null, "key");

            // Test case 10: Find user with Optional
            System.out.println("\n🔵 Test 10: Find user with Optional");
            Optional<User> found = secure.findUser("U001");
            found.ifPresent(u -> System.out.println("  Found: " + u.getDisplayName()));

            // Test case 11: Admin check
            System.out.println("\n🔵 Test 11: Admin check");
            System.out.println("  Null user admin? " + secure.isAdmin(null));
            System.out.println("  Regular user admin? " + secure.isAdmin(incompleteUser));

            // Test case 12: Safe unboxing
            System.out.println("\n🔵 Test 12: Safe unboxing");
            Optional<Integer> pref = secure.getPreferenceAsInt(user, "nonexistent");
            System.out.println("  Preference: " + pref.orElse(0));
        }

        public static void demonstrateNullSafeUtilities() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("NULL-SAFE UTILITIES DEMONSTRATION");
            System.out.println("=".repeat(60));

            // Test orDefault
            System.out.println("\n🔵 orDefault:");
            String str = null;
            String result = NullSafe.orDefault(str, "default");
            System.out.println("  null -> " + result);

            // Test safeToString
            System.out.println("\n🔵 safeToString:");
            Object obj = null;
            System.out.println("  null -> " + NullSafe.safeToString(obj));

            // Test safeEquals
            System.out.println("\n🔵 safeEquals:");
            System.out.println("  null == null: " + NullSafe.safeEquals(null, null));
            System.out.println("  null == 'a': " + NullSafe.safeEquals(null, "a"));
            System.out.println("  'a' == null: " + NullSafe.safeEquals("a", null));
            System.out.println("  'a' == 'a': " + NullSafe.safeEquals("a", "a"));

            // Test safeList
            System.out.println("\n🔵 safeList:");
            List<String> nullList = null;
            List<String> safeList = NullSafe.safeList(nullList);
            System.out.println("  Null list size: " + safeList.size());

            // Test safeMap
            System.out.println("\n🔵 safeMap:");
            Map<String, String> nullMap = null;
            Map<String, String> safeMap = NullSafe.safeMap(nullMap);
            System.out.println("  Null map size: " + safeMap.size());

            // Test safeSubstring
            System.out.println("\n🔵 safeSubstring:");
            String test = "Hello World";
            System.out.println("  Normal: " + NullSafe.safeSubstring(test, 0, 5));
            System.out.println("  Null string: " + NullSafe.safeSubstring(null, 0, 5));
            System.out.println("  Invalid range: " + NullSafe.safeSubstring(test, 10, 5));
        }

        private static User createTestUser() {
            User user = new User("1", "john_doe");
            user.setEmail("john@example.com");

            Address address = new Address();
            address.setStreet("123 Main St");
            address.setCity("Springfield");
            address.setZipCode("12345");

            Country country = new Country("US", "United States");
            address.setCountry(country);

            user.setAddress(address);

            Profile profile = new Profile();
            profile.setBio("Software Developer");

            Settings settings = new Settings();
            settings.setTheme("dark");

            Language lang = new Language();
            lang.setCode("en");
            lang.setDisplayName("English");
            settings.setLanguage(lang);

            profile.setSettings(settings);
            user.setProfile(profile);

            user.getRoles().add("USER");
            user.getPreferences().put("theme", "dark");

            return user;
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-476: NULL Pointer Dereference");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE null handling (CWE-476)");
            System.out.println("  2. Demonstrate SECURE null handling (Mitigated)");
            System.out.println("  3. Demonstrate null-safe utilities");
            System.out.println("  4. Test interactive null scenarios");
            System.out.println("  5. Show security analysis");
            System.out.println("  6. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    NullDemonstrator.demonstrateInsecureNulls();
                    break;

                case "2":
                    NullDemonstrator.demonstrateSecureNulls();
                    break;

                case "3":
                    NullDemonstrator.demonstrateNullSafeUtilities();
                    break;

                case "4":
                    testInteractiveNulls(scanner);
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

    private static void testInteractiveNulls(Scanner scanner) {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("INTERACTIVE NULL TESTING");
        System.out.println("=".repeat(60));

        SecureNullExamples secure = new SecureNullExamples();

        System.out.println("\nEnter test values (or 'null' to test null):");

        System.out.print("Enter user ID: ");
        String id = scanner.nextLine().trim();
        if ("null".equalsIgnoreCase(id))
            id = null;

        System.out.print("Enter username: ");
        String username = scanner.nextLine().trim();
        if ("null".equalsIgnoreCase(username))
            username = null;

        System.out.print("Enter email: ");
        String email = scanner.nextLine().trim();
        if ("null".equalsIgnoreCase(email))
            email = null;

        // Create user based on input
        User user = null;
        if (id != null && username != null) {
            user = new User(id, username);
            if (email != null) {
                user.setEmail(email);
            }
        }

        System.out.println("\nTesting with user: " + (user == null ? "null" : user.getDisplayName()));

        // Test various methods
        secure.processUser(user);

        Optional<String> country = secure.getUserCountry(user);
        System.out.println("\nCountry: " + country.orElse("Unknown"));

        boolean isAdmin = secure.isAdmin(user);
        System.out.println("Is admin: " + isAdmin);

        String userEmail = secure.getUserEmail(user);
        System.out.println("Email: " + userEmail);
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-476");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-476 VULNERABLE):");
        System.out.println("  1. No Null Checks:");
        System.out.println("     • Method parameters not validated");
        System.out.println("     • Return values assumed non-null");
        System.out.println("     • Chained calls without checks");

        System.out.println("\n  2. Common Scenarios:");
        System.out.println("     • user.getAddress().getCountry().getName()");
        System.out.println("     • array.length on null array");
        System.out.println("     • list.iterator() on null list");
        System.out.println("     • map.get(key).toString()");
        System.out.println("     • Auto-unboxing null Integer");

        System.out.println("\n  3. Consequences:");
        System.out.println("     • NullPointerException");
        System.out.println("     • Application crash");
        System.out.println("     • Denial of service");
        System.out.println("     • Data corruption");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Explicit Null Checks:");
        System.out.println("     • if (user == null) return;");
        System.out.println("     • Validate parameters at method start");

        System.out.println("\n  2. Optional Usage:");
        System.out.println("     • Optional.ofNullable()");
        System.out.println("     • Optional.map() for chaining");
        System.out.println("     • orElse() for defaults");

        System.out.println("\n  3. Null-Safe Utilities:");
        System.out.println("     • Objects.requireNonNull()");
        System.out.println("     • Apache Commons Lang (StringUtils)");
        System.out.println("     • Custom null-safe helpers");

        System.out.println("\n  4. Defensive Programming:");
        System.out.println("     • Never assume non-null");
        System.out.println("     • Document nullability");
        System.out.println("     • Use @Nullable/@NonNull annotations");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Always validate method parameters");
        System.out.println("  2. Use Optional for return values that may be absent");
        System.out.println("  3. Check for null before method calls");
        System.out.println("  4. Use Objects.requireNonNull() for required parameters");
        System.out.println("  5. Avoid returning null - use Optional or empty collections");
        System.out.println("  6. Use null-safe utilities for common operations");
        System.out.println("  7. Document nullability with annotations");
        System.out.println("  8. Initialize fields to safe defaults");
        System.out.println("  9. Use ternary operators carefully");
        System.out.println("  10. Watch for auto-unboxing of null wrappers");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use Optional for return values");
        System.out.println("  • Validate all inputs at method start");
        System.out.println("  • Use Objects.requireNonNull() for required params");
        System.out.println("  • Never chain calls without null checks");
        System.out.println("  • Use @Nullable/@NonNull annotations");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  public String getCountry(User user) {");
        System.out.println("      return user.getAddress().getCountry().getName();");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // SECURE (traditional)");
        System.out.println("  public String getCountry(User user) {");
        System.out.println("      if (user == null) return null;");
        System.out.println("      Address addr = user.getAddress();");
        System.out.println("      if (addr == null) return null;");
        System.out.println("      Country country = addr.getCountry();");
        System.out.println("      return country != null ? country.getName() : null;");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // SECURE (Optional)");
        System.out.println("  public Optional<String> getCountry(User user) {");
        System.out.println("      return Optional.ofNullable(user)");
        System.out.println("          .map(User::getAddress)");
        System.out.println("          .map(Address::getCountry)");
        System.out.println("          .map(Country::getName);");
        System.out.println("  }");

        System.out.println("\n📊 NULL RISK ASSESSMENT:");
        System.out.println("  Operation               | Risk Level | Consequence");
        System.out.println("  ------------------------|------------|-------------");
        System.out.println("  Direct method call      |    HIGH    | Crash");
        System.out.println("  Array access            |    HIGH    | Crash");
        System.out.println("  Collection iteration    |    HIGH    | Crash");
        System.out.println("  Auto-unboxing           |    HIGH    | Crash");
        System.out.println("  String concatenation    |    LOW     | 'null' text");
        System.out.println("  instanceof check        |    LOW     | false");
        System.out.println("  equals on constant      |    LOW     | false");
        System.out.println("  Ternary with null check |    MEDIUM  | Depends");

        System.out.println("\n⚠️ REMEMBER: A single NullPointerException can crash your entire application!");
    }
}