package deepseek.java;

import java.io.*;
import java.lang.reflect.*;
import java.nio.file.*;
import java.util.*;
import java.util.regex.Pattern;
import java.security.*;
import java.security.cert.Certificate;
import java.net.URL;
import java.net.URLClassLoader;

public class cwe_470 {

    // Configuration
    private static final String CLASS_LIST_FILE = "allowed_classes.txt";
    private static final String CLASS_NAME_FILE = "class_to_load.txt";
    private static final String CLASS_DIRECTORY = "./classes";

    // Security constants
    private static final Pattern CLASS_NAME_PATTERN = Pattern
            .compile("^[a-zA-Z][a-zA-Z0-9_]*(\\.[a-zA-Z][a-zA-Z0-9_]*)*$");

    private static final Set<String> ALLOWED_PACKAGES = new HashSet<>(Arrays.asList(
            "com.myapp.",
            "java.util.",
            "java.lang."));

    private static final Set<String> BLOCKED_CLASSES = new HashSet<>(Arrays.asList(
            "java.lang.Runtime",
            "java.lang.ProcessBuilder",
            "java.lang.System",
            "java.io.File",
            "java.io.FileInputStream",
            "java.io.FileOutputStream",
            "java.net.Socket",
            "java.net.URLClassLoader"));

    // ==================== SAMPLE CLASSES FOR DEMONSTRATION ====================

    public interface Plugin {
        String getName();

        void execute(String... args);

        String getDescription();
    }

    public static class UserService implements Plugin {
        private String name = "UserService";
        private Map<String, String> users = new HashMap<>();

        public UserService() {
            users.put("admin", "Administrator");
            users.put("user1", "John Doe");
        }

        @Override
        public String getName() {
            return name;
        }

        @Override
        public void execute(String... args) {
            System.out.println("[UserService] Listing users:");
            users.forEach((key, value) -> System.out.println("  - " + key + ": " + value));
        }

        @Override
        public String getDescription() {
            return "Manages user accounts and profiles";
        }
    }

    public static class PaymentProcessor implements Plugin {
        private String name = "PaymentProcessor";

        @Override
        public String getName() {
            return name;
        }

        @Override
        public void execute(String... args) {
            System.out.println("[PaymentProcessor] Processing payment: " +
                    (args.length > 0 ? args[0] : "No amount specified"));
        }

        @Override
        public String getDescription() {
            return "Handles payment transactions securely";
        }
    }

    public static class LoggerService implements Plugin {
        private String name = "LoggerService";
        private List<String> logs = new ArrayList<>();

        @Override
        public String getName() {
            return name;
        }

        @Override
        public void execute(String... args) {
            if (args.length > 0) {
                logs.add(args[0]);
                System.out.println("[LoggerService] Logged: " + args[0]);
            }
        }

        @Override
        public String getDescription() {
            return "Logging service for application events";
        }
    }

    // ==================== MALICIOUS CLASS (FOR DEMONSTRATION) ====================

    public static class MaliciousPlugin implements Plugin {

        public MaliciousPlugin() {
            // Simulate malicious activity in constructor
            System.err.println("[!] MALICIOUS CODE EXECUTED IN CONSTRUCTOR!");
        }

        @Override
        public String getName() {
            return "MaliciousPlugin";
        }

        @Override
        public void execute(String... args) {
            try {
                System.err.println("[!] Executing malicious payload...");

                // Attempt to access system resources
                ProcessBuilder pb = new ProcessBuilder("ls", "-la");
                Process p = pb.start();

                BufferedReader reader = new BufferedReader(
                        new InputStreamReader(p.getInputStream()));
                String line;
                while ((line = reader.readLine()) != null) {
                    System.err.println("[!] " + line);
                }

                // Attempt to read sensitive files
                File sensitive = new File("/etc/passwd");
                if (sensitive.exists()) {
                    System.err.println("[!] Attempted to read: " + sensitive.getAbsolutePath());
                }

            } catch (Exception e) {
                System.err.println("[!] Malicious action failed: " + e.getMessage());
            }
        }

        @Override
        public String getDescription() {
            return "Malicious plugin for demonstration";
        }
    }

    // ==================== INSECURE CLASS LOADER (CWE-470 VULNERABLE)
    // ====================

    static class InsecureClassLoader {

        private File classDirectory;

        public InsecureClassLoader() {
            this.classDirectory = new File(CLASS_DIRECTORY);
            if (!classDirectory.exists()) {
                classDirectory.mkdirs();
            }
        }

        // INSECURE: No validation of class name input
        public Object loadClassFromFile(String fileName) {
            try {
                // Read class name from file
                String className = readClassNameFromFile(fileName);

                System.out.println("[INSECURE] Loading class: " + className);

                // INSECURE: Direct class loading without validation
                Class<?> clazz = Class.forName(className);

                // Create instance
                Constructor<?> constructor = clazz.getConstructor();
                Object instance = constructor.newInstance();

                System.out.println("[INSECURE] Successfully created instance of: " +
                        clazz.getSimpleName());

                return instance;

            } catch (ClassNotFoundException e) {
                System.err.println("[INSECURE] Class not found: " + e.getMessage());
                return null;
            } catch (NoSuchMethodException e) {
                System.err.println("[INSECURE] No default constructor: " + e.getMessage());
                return null;
            } catch (Exception e) {
                System.err.println("[INSECURE] Error creating instance: " + e.getMessage());
                e.printStackTrace();
                return null;
            }
        }

        // INSECURE: Dynamic class loading from external source
        public Object loadExternalClass(String className, byte[] classData) {
            try {
                // INSECURE: Custom class loader without security checks
                CustomClassLoader loader = new CustomClassLoader();
                Class<?> clazz = loader.defineClass(className, classData);

                Constructor<?> constructor = clazz.getConstructor();
                return constructor.newInstance();

            } catch (Exception e) {
                System.err.println("[INSECURE] Failed to load external class: " + e.getMessage());
                return null;
            }
        }

        private String readClassNameFromFile(String fileName) throws IOException {
            return Files.readString(Paths.get(fileName)).trim();
        }

        // Custom class loader (INSECURE - no validation)
        class CustomClassLoader extends ClassLoader {
            public Class<?> defineClass(String name, byte[] data) {
                return defineClass(name, data, 0, data.length);
            }
        }
    }

    // ==================== SECURE CLASS LOADER (CWE-470 MITIGATED)
    // ====================

    static class SecureClassLoader {

        private final Set<String> allowedClasses;
        private final SecurityManager securityManager;
        private final ClassLoader parentLoader;

        public SecureClassLoader() throws IOException {
            this.allowedClasses = loadAllowedClasses();
            this.securityManager = System.getSecurityManager();
            this.parentLoader = ClassLoader.getSystemClassLoader();

            // Set up security manager if not present
            if (securityManager == null) {
                System.setSecurityManager(new SecurityManager() {
                    @Override
                    public void checkPermission(Permission perm) {
                        // Restrict sensitive operations
                        if (perm.getName().startsWith("exitVM") ||
                                perm.getName().equals("setSecurityManager") ||
                                perm.getName().contains("file") &&
                                        !perm.getName().contains("read")) {
                            throw new SecurityException("Operation not allowed: " + perm.getName());
                        }
                    }

                    @Override
                    public void checkPackageAccess(String pkg) {
                        // Prevent access to sensitive packages
                        if (pkg.startsWith("java.lang.reflect") ||
                                pkg.startsWith("sun.reflect") ||
                                pkg.startsWith("java.security")) {
                            throw new SecurityException("Package access denied: " + pkg);
                        }
                    }
                });
            }
        }

        private Set<String> loadAllowedClasses() throws IOException {
            Set<String> classes = new HashSet<>();
            File allowedFile = new File(CLASS_LIST_FILE);

            if (!allowedFile.exists()) {
                // Create default allowed classes file
                createDefaultAllowedClassesFile();
            }

            List<String> lines = Files.readAllLines(allowedFile.toPath());
            for (String line : lines) {
                line = line.trim();
                if (!line.isEmpty() && !line.startsWith("#")) {
                    classes.add(line);
                }
            }

            return classes;
        }

        private void createDefaultAllowedClassesFile() throws IOException {
            List<String> defaultClasses = Arrays.asList(
                    "# Allowed classes for CWE-470 demonstration",
                    "# Format: fully qualified class names, one per line",
                    "",
                    "cwe_470$UserService",
                    "cwe_470$PaymentProcessor",
                    "cwe_470$LoggerService",
                    "java.util.ArrayList",
                    "java.util.HashMap",
                    "java.util.Date");

            Files.write(Paths.get(CLASS_LIST_FILE), defaultClasses);
            System.out.println("[SECURE] Created default allowed classes file: " + CLASS_LIST_FILE);
        }

        // SECURE: Validate class name format
        private boolean validateClassName(String className) {
            if (className == null || className.trim().isEmpty()) {
                System.err.println("[SECURE] Class name is empty");
                return false;
            }

            // Check format
            if (!CLASS_NAME_PATTERN.matcher(className).matches()) {
                System.err.println("[SECURE] Invalid class name format: " + className);
                return false;
            }

            // Check if class is blocked
            if (BLOCKED_CLASSES.contains(className)) {
                System.err.println("[SECURE] Class is blocked: " + className);
                return false;
            }

            // Check if class is in allowed list
            if (!allowedClasses.contains(className)) {
                System.err.println("[SECURE] Class not in allowed list: " + className);
                return false;
            }

            // Check package access
            for (String allowedPackage : ALLOWED_PACKAGES) {
                if (className.startsWith(allowedPackage)) {
                    return true;
                }
            }

            // Allow classes from same package (our demo classes)
            if (className.startsWith("cwe_470$")) {
                return true;
            }

            System.err.println("[SECURE] Class package not allowed: " + className);
            return false;
        }

        // SECURE: Validate that class implements expected interface
        private boolean validateClassImplements(Class<?> clazz, Class<?> expectedInterface) {
            return expectedInterface.isAssignableFrom(clazz);
        }

        // SECURE: Load class with validation
        public Object loadClassFromFile(String fileName) {
            try {
                // Read class name from file
                String className = readClassNameFromFile(fileName);

                System.out.println("[SECURE] Attempting to load class: " + className);

                // SECURE: Validate class name
                if (!validateClassName(className)) {
                    throw new SecurityException("Class name validation failed");
                }

                // SECURE: Use context class loader with security manager
                Class<?> clazz = Class.forName(className, true,
                        AccessController.doPrivileged(new PrivilegedAction<ClassLoader>() {
                            public ClassLoader run() {
                                return Thread.currentThread().getContextClassLoader();
                            }
                        }));

                // SECURE: Validate class implements expected interface
                if (!validateClassImplements(clazz, Plugin.class)) {
                    throw new SecurityException("Class does not implement Plugin interface");
                }

                // SECURE: Check constructor accessibility
                Constructor<?> constructor = clazz.getDeclaredConstructor();
                if (!Modifier.isPublic(constructor.getModifiers())) {
                    throw new SecurityException("Constructor is not public");
                }

                // SECURE: Create instance with privilege restriction
                Object instance = AccessController.doPrivileged(
                        new PrivilegedExceptionAction<Object>() {
                            public Object run() throws Exception {
                                return constructor.newInstance();
                            }
                        });

                System.out.println("[SECURE] Successfully created validated instance of: " +
                        clazz.getSimpleName());

                return instance;

            } catch (PrivilegedActionException e) {
                System.err.println("[SECURE] Privileged action failed: " + e.getCause().getMessage());
                return null;
            } catch (SecurityException e) {
                System.err.println("[SECURE] Security violation: " + e.getMessage());
                return null;
            } catch (Exception e) {
                System.err.println("[SECURE] Error: " + e.getMessage());
                return null;
            }
        }

        // SECURE: Load class with signature verification
        public Object loadVerifiedClass(String className, byte[] classData, byte[] signature) {
            try {
                // SECURE: Verify class signature
                if (!verifyClassSignature(classData, signature)) {
                    throw new SecurityException("Class signature verification failed");
                }

                // SECURE: Validate class name
                if (!validateClassName(className)) {
                    throw new SecurityException("Class name validation failed");
                }

                // SECURE: Use secure class loader with validation
                VerifiedClassLoader loader = new VerifiedClassLoader();
                Class<?> clazz = loader.defineVerifiedClass(className, classData);

                // Validate interface
                if (!validateClassImplements(clazz, Plugin.class)) {
                    throw new SecurityException("Class does not implement Plugin interface");
                }

                Constructor<?> constructor = clazz.getConstructor();
                return constructor.newInstance();

            } catch (Exception e) {
                System.err.println("[SECURE] Failed to load verified class: " + e.getMessage());
                return null;
            }
        }

        private boolean verifyClassSignature(byte[] classData, byte[] signature) {
            // In production, implement proper signature verification
            // using digital signatures and trusted certificates
            return signature != null && signature.length > 0;
        }

        private String readClassNameFromFile(String fileName) throws IOException {
            return Files.readString(Paths.get(fileName)).trim();
        }

        // SECURE: Class loader with validation
        class VerifiedClassLoader extends ClassLoader {

            public Class<?> defineVerifiedClass(String name, byte[] data) {
                // SECURE: Verify class bytes before defining
                if (!verifyClassBytes(data)) {
                    throw new SecurityException("Class byte verification failed");
                }

                // SECURE: Define class with protection domain
                ProtectionDomain domain = new ProtectionDomain(
                        new CodeSource(null, (Certificate[]) null),
                        new Permissions(),
                        this,
                        null);

                return defineClass(name, data, 0, data.length, domain);
            }

            private boolean verifyClassBytes(byte[] data) {
                // Simple verification - check for suspicious bytecode patterns
                // In production, use a proper bytecode verifier
                String dataStr = new String(data);

                // Check for dangerous method calls
                if (dataStr.contains("java/lang/Runtime") ||
                        dataStr.contains("java/lang/ProcessBuilder") ||
                        dataStr.contains("java/io/FileOutputStream")) {
                    return false;
                }

                return true;
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-470: Use of Externally-Controlled Input to Select Classes");
        System.out.println("================================================\n");

        // Create sample class name file
        createSampleClassFile();

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE class loading (CWE-470)");
            System.out.println("  2. Demonstrate SECURE class loading (Mitigated)");
            System.out.println("  3. Demonstrate malicious class loading (INSECURE)");
            System.out.println("  4. Show allowed classes list");
            System.out.println("  5. Test class validation");
            System.out.println("  6. Show security analysis");
            System.out.println("  7. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecureLoading();
                    break;

                case "2":
                    demonstrateSecureLoading();
                    break;

                case "3":
                    demonstrateMaliciousLoading();
                    break;

                case "4":
                    showAllowedClasses();
                    break;

                case "5":
                    testClassValidation(scanner);
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

    private static void createSampleClassFile() {
        try {
            Files.writeString(Paths.get(CLASS_NAME_FILE),
                    "cwe_470$UserService\n");
            System.out.println("[✓] Created sample class name file: " + CLASS_NAME_FILE);
        } catch (IOException e) {
            System.err.println("[!] Could not create sample file: " + e.getMessage());
        }
    }

    private static void demonstrateInsecureLoading() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE CLASS LOADING DEMONSTRATION");
        System.out.println("=".repeat(50));

        InsecureClassLoader loader = new InsecureClassLoader();

        // Load legitimate class
        System.out.println("\n--- Loading legitimate class ---");
        Object userService = loader.loadClassFromFile(CLASS_NAME_FILE);

        if (userService instanceof Plugin) {
            Plugin plugin = (Plugin) userService;
            System.out.println("\nPlugin details:");
            System.out.println("  Name: " + plugin.getName());
            System.out.println("  Description: " + plugin.getDescription());
            System.out.println("  Executing:");
            plugin.execute();
        }

        // Try to load non-existent class
        System.out.println("\n--- Attempting to load non-existent class ---");
        loader.loadClassFromFile("nonexistent.txt");
    }

    private static void demonstrateSecureLoading() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SECURE CLASS LOADING DEMONSTRATION");
        System.out.println("=".repeat(50));

        try {
            SecureClassLoader loader = new SecureClassLoader();

            // Load legitimate class
            System.out.println("\n--- Loading legitimate class ---");
            Object userService = loader.loadClassFromFile(CLASS_NAME_FILE);

            if (userService instanceof Plugin) {
                Plugin plugin = (Plugin) userService;
                System.out.println("\nPlugin details:");
                System.out.println("  Name: " + plugin.getName());
                System.out.println("  Description: " + plugin.getDescription());
                System.out.println("  Executing:");
                plugin.execute();
            }

            // Try to load class not in allowed list
            System.out.println("\n--- Attempting to load disallowed class ---");
            Files.writeString(Paths.get("disallowed.txt"), "java.lang.Runtime");
            loader.loadClassFromFile("disallowed.txt");

        } catch (IOException e) {
            System.err.println("[SECURE] Error: " + e.getMessage());
        }
    }

    private static void demonstrateMaliciousLoading() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("MALICIOUS CLASS LOADING DEMONSTRATION");
        System.out.println("=".repeat(50));

        // Create file with malicious class name
        try {
            Files.writeString(Paths.get("malicious.txt"),
                    "cwe_470$MaliciousPlugin");
            System.out.println("[!] Created malicious class reference file");
        } catch (IOException e) {
            System.err.println("Failed to create malicious file: " + e.getMessage());
        }

        // INSECURE loader - will load malicious class
        System.out.println("\n--- INSECURE loader (VULNERABLE) ---");
        InsecureClassLoader insecureLoader = new InsecureClassLoader();
        Object malicious = insecureLoader.loadClassFromFile("malicious.txt");

        if (malicious instanceof Plugin) {
            Plugin plugin = (Plugin) malicious;
            System.out.println("\nMalicious plugin loaded:");
            System.out.println("  Name: " + plugin.getName());
            System.out.println("  Description: " + plugin.getDescription());
            System.out.println("  Executing:");
            plugin.execute();
        }

        // SECURE loader - should block malicious class
        System.out.println("\n--- SECURE loader (PROTECTED) ---");
        try {
            SecureClassLoader secureLoader = new SecureClassLoader();
            secureLoader.loadClassFromFile("malicious.txt");
        } catch (Exception e) {
            System.err.println("Secure loader blocked malicious class");
        }
    }

    private static void showAllowedClasses() {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("ALLOWED CLASSES");
        System.out.println("=".repeat(50));

        try {
            SecureClassLoader loader = new SecureClassLoader();

            System.out.println("\nClasses allowed for loading:");
            loader.getClass().getDeclaredField("allowedClasses")
                    .setAccessible(true);

            @SuppressWarnings("unchecked")
            Set<String> allowed = (Set<String>) loader.getClass()
                    .getDeclaredField("allowedClasses")
                    .get(loader);

            allowed.stream().sorted().forEach(cls -> System.out.println("  • " + cls));

            System.out.println("\nBlocked classes:");
            BLOCKED_CLASSES.stream().sorted().forEach(cls -> System.out.println("  • " + cls));

        } catch (Exception e) {
            System.err.println("Error showing allowed classes: " + e.getMessage());
        }
    }

    private static void testClassValidation(Scanner scanner) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("CLASS VALIDATION TEST");
        System.out.println("=".repeat(50));

        System.out.print("\nEnter class name to test: ");
        String className = scanner.nextLine().trim();

        try {
            SecureClassLoader loader = new SecureClassLoader();

            // Test validation
            Files.writeString(Paths.get("test.txt"), className);
            Object instance = loader.loadClassFromFile("test.txt");

            if (instance != null) {
                System.out.println("✅ Class validated and loaded successfully");
                if (instance instanceof Plugin) {
                    Plugin p = (Plugin) instance;
                    System.out.println("  Plugin: " + p.getName());
                }
            } else {
                System.out.println("❌ Class validation failed (as expected)");
            }

        } catch (Exception e) {
            System.out.println("❌ Validation error: " + e.getMessage());
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-470");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-470 VULNERABLE):");
        System.out.println("  • No validation of class names");
        System.out.println("  • Can load any class from classpath");
        System.out.println("  • Can load malicious classes");
        System.out.println("  • No interface contract enforcement");
        System.out.println("  • No security manager");
        System.out.println("  • Can access sensitive system classes");
        System.out.println("  • Can execute arbitrary code");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Input Validation:");
        System.out.println("     • Class name format validation");
        System.out.println("     • Whitelist of allowed classes");
        System.out.println("     • Blocklist of dangerous classes");

        System.out.println("\n  2. Interface Enforcement:");
        System.out.println("     • Classes must implement Plugin interface");
        System.out.println("     • Contract-based execution");

        System.out.println("\n  3. Security Manager:");
        System.out.println("     • Restricts sensitive operations");
        System.out.println("     • Controls package access");
        System.out.println("     • Prevents privilege escalation");

        System.out.println("\n  4. Class Loading Controls:");
        System.out.println("     • Verified class bytes");
        System.out.println("     • Protection domains");
        System.out.println("     • Signature verification");

        System.out.println("\n  5. Constructor Validation:");
        System.out.println("     • Public constructor required");
        System.out.println("     • Privileged action wrapper");

        System.out.println("\n📋 BEST PRACTICES FOR CWE-470:");
        System.out.println("  1. Never load classes from untrusted input");
        System.out.println("  2. Use strict whitelist of allowed classes");
        System.out.println("  3. Require interface implementation");
        System.out.println("  4. Use SecurityManager");
        System.out.println("  5. Validate class format and package");
        System.out.println("  6. Verify class signatures");
        System.out.println("  7. Use separate classloaders for untrusted code");
        System.out.println("  8. Implement bytecode verification");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use plugin architecture with interfaces");
        System.out.println("  • Maintain strict class whitelist");
        System.out.println("  • Enable SecurityManager");
        System.out.println("  • Sign and verify trusted classes");
        System.out.println("  • Never load from untrusted sources");
    }
}