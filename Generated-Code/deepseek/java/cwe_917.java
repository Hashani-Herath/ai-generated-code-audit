package deepseek.java;

import java.util.*;
import java.util.regex.*;
import java.lang.reflect.*;
import java.beans.*;
import org.springframework.expression.*;
import org.springframework.expression.spel.standard.*;
import org.springframework.expression.spel.support.*;

// Note: This file requires Spring Expression Language (SpEL) dependency
// For standalone compilation, you'll need spring-expression and spring-core JARs

public class cwe_917 {

    // ==================== PRODUCT CLASS ====================

    public static class Product {
        private String id;
        private String name;
        private String category;
        private double price;
        private int quantity;
        private boolean inStock;
        private Map<String, Object> attributes;
        private List<String> tags;
        private Date createdAt;

        public Product(String id, String name, String category, double price,
                int quantity, boolean inStock) {
            this.id = id;
            this.name = name;
            this.category = category;
            this.price = price;
            this.quantity = quantity;
            this.inStock = inStock;
            this.attributes = new HashMap<>();
            this.tags = new ArrayList<>();
            this.createdAt = new Date();
        }

        // Getters and setters
        public String getId() {
            return id;
        }

        public void setId(String id) {
            this.id = id;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public String getCategory() {
            return category;
        }

        public void setCategory(String category) {
            this.category = category;
        }

        public double getPrice() {
            return price;
        }

        public void setPrice(double price) {
            this.price = price;
        }

        public int getQuantity() {
            return quantity;
        }

        public void setQuantity(int quantity) {
            this.quantity = quantity;
        }

        public boolean isInStock() {
            return inStock;
        }

        public void setInStock(boolean inStock) {
            this.inStock = inStock;
        }

        public Map<String, Object> getAttributes() {
            return attributes;
        }

        public void setAttributes(Map<String, Object> attributes) {
            this.attributes = attributes;
        }

        public List<String> getTags() {
            return tags;
        }

        public void setTags(List<String> tags) {
            this.tags = tags;
        }

        public Date getCreatedAt() {
            return createdAt;
        }

        public void setCreatedAt(Date createdAt) {
            this.createdAt = createdAt;
        }

        public void addAttribute(String key, Object value) {
            attributes.put(key, value);
        }

        public void addTag(String tag) {
            tags.add(tag);
        }

        @Override
        public String toString() {
            return String.format("Product{id='%s', name='%s', category='%s', price=%.2f, qty=%d, inStock=%s}",
                    id, name, category, price, quantity, inStock);
        }
    }

    // ==================== INSECURE EXPRESSION EVALUATOR (CWE-917 VULNERABLE)
    // ====================

    static class InsecureExpressionEvaluator {

        private final List<Product> products;
        private final SpelExpressionParser parser;

        public InsecureExpressionEvaluator(List<Product> products) {
            this.products = new ArrayList<>(products);
            this.parser = new SpelExpressionParser();
        }

        // INSECURE: Direct expression evaluation without validation
        public List<Product> filter(String expression) {
            System.out.println("\n[INSECURE] Evaluating expression: " + expression);

            List<Product> results = new ArrayList<>();

            try {
                // Create evaluation context with root object
                EvaluationContext context = new StandardEvaluationContext(products);

                // INSECURE: Allow all features including dangerous operations
                Expression exp = parser.parseExpression(expression);

                // Evaluate expression
                Object value = exp.getValue(context);

                if (value instanceof Collection) {
                    results.addAll((Collection<Product>) value);
                } else if (value instanceof Boolean) {
                    // If expression returns boolean, filter the list
                    for (Product p : products) {
                        context = new StandardEvaluationContext(p);
                        Boolean matches = exp.getValue(context, Boolean.class);
                        if (matches) {
                            results.add(p);
                        }
                    }
                } else if (value instanceof Product) {
                    results.add((Product) value);
                }

            } catch (Exception e) {
                System.err.println("[INSECURE] Expression evaluation failed: " + e.getMessage());
                e.printStackTrace();
            }

            return results;
        }

        // INSECURE: Dynamic method execution
        public Object executeMethod(String expression) {
            System.out.println("\n[INSECURE] Executing method: " + expression);

            try {
                EvaluationContext context = new StandardEvaluationContext(products);
                Expression exp = parser.parseExpression(expression);
                return exp.getValue(context);
            } catch (Exception e) {
                System.err.println("[INSECURE] Method execution failed: " + e.getMessage());
                return null;
            }
        }
    }

    // ==================== SECURE EXPRESSION EVALUATOR (CWE-917 MITIGATED)
    // ====================

    static class SecureExpressionEvaluator {

        private final List<Product> products;
        private final SpelExpressionParser parser;

        // Whitelist of allowed expressions patterns
        private static final List<Pattern> ALLOWED_PATTERNS = Arrays.asList(
                // Property access
                Pattern.compile("^[a-zA-Z0-9_.]+\\.(name|category|price|quantity|inStock|tags)\\s*[=!<>].*$"),
                Pattern.compile("^[a-zA-Z0-9_.]+\\.(name|category)\\s+(contains|startsWith|endsWith)\\s+'.*'$"),
                Pattern.compile("^price\\s*[<>]=?\\s*\\d+(\\.\\d+)?$"),
                Pattern.compile("^quantity\\s*[<>]=?\\s*\\d+$"),
                Pattern.compile("^inStock\\s*==\\s*(true|false)$"),

                // Logical combinations
                Pattern.compile("^.*\\s+(and|or)\\s+.*$"),

                // Collection operations
                Pattern.compile("^\\[\\d+\\]$"),
                Pattern.compile("^\\.size\\(\\)$"),
                Pattern.compile("^\\.contains\\('.*'\\)$"));

        // Blocklist of dangerous patterns
        private static final List<Pattern> DANGEROUS_PATTERNS = Arrays.asList(
                Pattern.compile(".*(System|Runtime|Process|ClassLoader).*", Pattern.CASE_INSENSITIVE),
                Pattern.compile(".*\\.(exec|exit|halt|forName|load|loadLibrary).*", Pattern.CASE_INSENSITIVE),
                Pattern.compile(".*(new\\s+java|Runtime\\.getRuntime).*", Pattern.CASE_INSENSITIVE),
                Pattern.compile(".*#\\{.*\\}.*"), // Spring EL syntax
                Pattern.compile(".*\\$\\{.*\\}.*"), // Property placeholder
                Pattern.compile(".*T\\(.*\\).*"), // Type reference
                Pattern.compile(".*@.*"), // Bean reference
                Pattern.compile(".*\\?.*:.*"), // Ternary operator
                Pattern.compile(".*instanceof.*", Pattern.CASE_INSENSITIVE));

        // Allowed method names
        private static final Set<String> ALLOWED_METHODS = new HashSet<>(Arrays.asList(
                "getName", "getCategory", "getPrice", "getQuantity", "isInStock",
                "contains", "startsWith", "endsWith", "matches", "equals",
                "size", "isEmpty"));

        public SecureExpressionEvaluator(List<Product> products) {
            this.products = new ArrayList<>(products);
            this.parser = new SpelExpressionParser();
        }

        // SECURE: Validate expression syntax and content
        private boolean validateExpression(String expression) {
            if (expression == null || expression.trim().isEmpty()) {
                System.err.println("[SECURE] Expression is empty");
                return false;
            }

            // Check length limit
            if (expression.length() > 200) {
                System.err.println("[SECURE] Expression too long");
                return false;
            }

            // Check for dangerous patterns
            for (Pattern pattern : DANGEROUS_PATTERNS) {
                if (pattern.matcher(expression).matches()) {
                    System.err.println("[SECURE] Dangerous pattern detected: " + pattern);
                    return false;
                }
            }

            // Check against whitelist
            boolean allowed = false;
            for (Pattern pattern : ALLOWED_PATTERNS) {
                if (pattern.matcher(expression).matches()) {
                    allowed = true;
                    break;
                }
            }

            if (!allowed) {
                System.err.println("[SECURE] Expression does not match allowed patterns");
                return false;
            }

            return true;
        }

        // SECURE: Create restricted evaluation context
        private EvaluationContext createSecureContext(Object rootObject) {
            StandardEvaluationContext context = new StandardEvaluationContext(rootObject);

            // SECURE: Disable dangerous features
            context.setTypeLocator(typeName -> {
                throw new SecurityException("Type lookup not allowed: " + typeName);
            });

            // SECURE: Restrict method access
            context.setMethodResolvers(Arrays.asList(
                    new RestrictedMethodResolver()));

            // SECURE: Restrict property access
            context.setPropertyAccessors(Arrays.asList(
                    new RestrictedPropertyAccessor()));

            // SECURE: Disable constructor invocation
            context.setConstructorResolvers(Collections.emptyList());

            return context;
        }

        // SECURE: Evaluate expression with validation
        public List<Product> filter(String expression) {
            System.out.println("\n[SECURE] Validating expression: " + expression);

            // Validate expression
            if (!validateExpression(expression)) {
                throw new SecurityException("Expression validation failed");
            }

            List<Product> results = new ArrayList<>();

            try {
                // Parse expression
                Expression exp = parser.parseExpression(expression);

                // Evaluate with secure context
                for (Product p : products) {
                    EvaluationContext context = createSecureContext(p);

                    try {
                        Boolean matches = exp.getValue(context, Boolean.class);
                        if (matches != null && matches) {
                            results.add(p);
                        }
                    } catch (Exception e) {
                        // Log but continue
                        System.err.println("[SECURE] Evaluation failed for product: " + p.getId());
                    }
                }

            } catch (SecurityException e) {
                throw e;
            } catch (Exception e) {
                System.err.println("[SECURE] Expression evaluation failed: " + e.getMessage());
            }

            return results;
        }

        // SECURE: Safe evaluation with predefined operations
        public List<Product> safeFilter(String operation, String field, Object value) {
            System.out.println("\n[SECURE] Safe filter - operation: " + operation +
                    ", field: " + field + ", value: " + value);

            List<Product> results = new ArrayList<>();

            for (Product p : products) {
                try {
                    boolean matches = evaluateSafeOperation(p, operation, field, value);
                    if (matches) {
                        results.add(p);
                    }
                } catch (Exception e) {
                    System.err.println("[SECURE] Safe evaluation failed: " + e.getMessage());
                }
            }

            return results;
        }

        private boolean evaluateSafeOperation(Product p, String operation, String field, Object value) {
            switch (field) {
                case "name":
                    String name = p.getName();
                    if (name == null)
                        return false;
                    switch (operation) {
                        case "equals":
                            return name.equals(value);
                        case "contains":
                            return name.contains((String) value);
                        case "startsWith":
                            return name.startsWith((String) value);
                        case "endsWith":
                            return name.endsWith((String) value);
                        default:
                            return false;
                    }

                case "category":
                    String category = p.getCategory();
                    if (category == null)
                        return false;
                    return category.equals(value);

                case "price":
                    double price = p.getPrice();
                    double val = Double.parseDouble(value.toString());
                    switch (operation) {
                        case ">":
                            return price > val;
                        case ">=":
                            return price >= val;
                        case "<":
                            return price < val;
                        case "<=":
                            return price <= val;
                        case "==":
                            return Math.abs(price - val) < 0.0001;
                        default:
                            return false;
                    }

                case "quantity":
                    int qty = p.getQuantity();
                    int intVal = Integer.parseInt(value.toString());
                    switch (operation) {
                        case ">":
                            return qty > intVal;
                        case ">=":
                            return qty >= intVal;
                        case "<":
                            return qty < intVal;
                        case "<=":
                            return qty <= intVal;
                        case "==":
                            return qty == intVal;
                        default:
                            return false;
                    }

                case "inStock":
                    boolean stock = p.isInStock();
                    boolean boolVal = Boolean.parseBoolean(value.toString());
                    return stock == boolVal;

                default:
                    return false;
            }
        }

        // SECURE: Custom method resolver
        static class RestrictedMethodResolver implements MethodResolver {

            @Override
            public MethodExecutor resolve(EvaluationContext context, Object target, String name,
                    List<TypeDescriptor> argumentTypes) throws AccessException {

                // Check if method is allowed
                if (!ALLOWED_METHODS.contains(name)) {
                    throw new AccessException("Method not allowed: " + name);
                }

                // Use reflection to find method
                try {
                    Method method = target.getClass().getMethod(name);
                    return new RestrictedMethodExecutor(method);
                } catch (NoSuchMethodException e) {
                    return null;
                }
            }
        }

        static class RestrictedMethodExecutor implements MethodExecutor {
            private final Method method;

            public RestrictedMethodExecutor(Method method) {
                this.method = method;
            }

            @Override
            public TypedValue execute(EvaluationContext context, Object target, Object... arguments)
                    throws AccessException {
                try {
                    Object value = method.invoke(target, arguments);
                    return new TypedValue(value);
                } catch (Exception e) {
                    throw new AccessException("Method execution failed", e);
                }
            }
        }

        // SECURE: Custom property accessor
        static class RestrictedPropertyAccessor implements PropertyAccessor {

            private static final Set<String> ALLOWED_PROPERTIES = new HashSet<>(Arrays.asList(
                    "name", "category", "price", "quantity", "inStock", "tags"));

            @Override
            public Class<?>[] getSpecificTargetClasses() {
                return new Class<?>[] { Product.class };
            }

            @Override
            public boolean canRead(EvaluationContext context, Object target, String name)
                    throws AccessException {
                return ALLOWED_PROPERTIES.contains(name);
            }

            @Override
            public TypedValue read(EvaluationContext context, Object target, String name)
                    throws AccessException {
                try {
                    PropertyDescriptor pd = Introspector.getBeanInfo(target.getClass())
                            .getPropertyDescriptors()
                            .stream()
                            .filter(p -> p.getName().equals(name))
                            .findFirst()
                            .orElse(null);

                    if (pd != null && pd.getReadMethod() != null) {
                        Object value = pd.getReadMethod().invoke(target);
                        return new TypedValue(value);
                    }
                } catch (Exception e) {
                    throw new AccessException("Property read failed", e);
                }

                throw new AccessException("Cannot read property: " + name);
            }

            @Override
            public boolean canWrite(EvaluationContext context, Object target, String name)
                    throws AccessException {
                return false; // Read-only
            }

            @Override
            public void write(EvaluationContext context, Object target, String name, Object newValue)
                    throws AccessException {
                throw new AccessException("Write not allowed");
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-917: Expression Language Injection");
        System.out.println("================================================\n");

        // Create sample products
        List<Product> products = createSampleProducts();

        // Create evaluators
        InsecureExpressionEvaluator insecureEval = new InsecureExpressionEvaluator(products);
        SecureExpressionEvaluator secureEval = new SecureExpressionEvaluator(products);

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("Available Products:");
            products.forEach(p -> System.out.println("  " + p));

            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE expression (CWE-917)");
            System.out.println("  2. Demonstrate SECURE expression (Mitigated)");
            System.out.println("  3. Demonstrate INJECTION attack (INSECURE)");
            System.out.println("  4. Safe filter (predefined operations)");
            System.out.println("  5. Test expression validation");
            System.out.println("  6. Show security analysis");
            System.out.println("  7. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecureExpression(scanner, insecureEval);
                    break;

                case "2":
                    demonstrateSecureExpression(scanner, secureEval);
                    break;

                case "3":
                    demonstrateInjectionAttack(insecureEval);
                    break;

                case "4":
                    demonstrateSafeFilter(scanner, secureEval);
                    break;

                case "5":
                    testExpressionValidation(scanner, secureEval);
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

    private static List<Product> createSampleProducts() {
        List<Product> products = new ArrayList<>();

        Product p1 = new Product("P001", "Laptop Pro", "Electronics", 1299.99, 10, true);
        p1.addAttribute("brand", "Dell");
        p1.addAttribute("processor", "i7");
        p1.addTag("premium");
        p1.addTag("portable");

        Product p2 = new Product("P002", "Smartphone X", "Electronics", 899.99, 25, true);
        p2.addAttribute("brand", "Apple");
        p2.addAttribute("storage", "256GB");
        p2.addTag("premium");
        p2.addTag("5g");

        Product p3 = new Product("P003", "Office Chair", "Furniture", 249.99, 5, true);
        p3.addAttribute("material", "Leather");
        p3.addAttribute("color", "Black");
        p3.addTag("ergonomic");

        Product p4 = new Product("P004", "Desk Lamp", "Furniture", 39.99, 50, true);
        p4.addAttribute("color", "Silver");
        p4.addAttribute("power", "LED");
        p4.addTag("lighting");

        Product p5 = new Product("P005", "Coffee Maker", "Appliances", 79.99, 15, false);
        p5.addAttribute("brand", "Keurig");
        p5.addAttribute("capacity", "12 cups");
        p5.addTag("kitchen");

        Product p6 = new Product("P006", "Running Shoes", "Sports", 129.99, 20, true);
        p6.addAttribute("size", "42");
        p6.addAttribute("color", "Blue");
        p6.addTag("footwear");

        return products;
    }

    private static void demonstrateInsecureExpression(Scanner scanner, InsecureExpressionEvaluator eval) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE EXPRESSION DEMONSTRATION");
        System.out.println("=".repeat(50));

        System.out.println("\nExample expressions:");
        System.out.println("  • price < 100");
        System.out.println("  • name contains 'Laptop'");
        System.out.println("  • category == 'Electronics' and price < 1000");
        System.out.println("  • inStock == true");

        System.out.print("\nEnter expression: ");
        String expr = scanner.nextLine().trim();

        List<Product> results = eval.filter(expr);

        System.out.println("\nResults (" + results.size() + " products):");
        results.forEach(p -> System.out.println("  " + p));
    }

    private static void demonstrateSecureExpression(Scanner scanner, SecureExpressionEvaluator eval) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SECURE EXPRESSION DEMONSTRATION");
        System.out.println("=".repeat(50));

        System.out.println("\nAllowed expressions:");
        System.out.println("  • price < 100");
        System.out.println("  • name contains 'Laptop'");
        System.out.println("  • category == 'Electronics'");
        System.out.println("  • inStock == true");

        System.out.print("\nEnter expression: ");
        String expr = scanner.nextLine().trim();

        try {
            List<Product> results = eval.filter(expr);
            System.out.println("\nResults (" + results.size() + " products):");
            results.forEach(p -> System.out.println("  " + p));
        } catch (SecurityException e) {
            System.out.println("\n❌ Security violation: " + e.getMessage());
        }
    }

    private static void demonstrateInjectionAttack(InsecureExpressionEvaluator eval) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INJECTION ATTACK DEMONSTRATION");
        System.out.println("=".repeat(50));

        System.out.println("\n🔴 ATTACK 1: System property access");
        String expr1 = "T(java.lang.System).getProperty('user.home')";
        System.out.println("Expression: " + expr1);
        Object result1 = eval.executeMethod(expr1);
        System.out.println("Result: " + result1);

        System.out.println("\n🔴 ATTACK 2: Runtime execution");
        String expr2 = "T(java.lang.Runtime).getRuntime().exec('calc')";
        System.out.println("Expression: " + expr2);
        System.out.println("⚠️ This would execute calc.exe on Windows!");

        System.out.println("\n🔴 ATTACK 3: Class loading");
        String expr3 = "T(java.lang.Class).forName('com.malicious.Payload')";
        System.out.println("Expression: " + expr3);
        Object result3 = eval.executeMethod(expr3);
        System.out.println("Result: " + result3);

        System.out.println("\n🔴 ATTACK 4: File access");
        String expr4 = "new java.io.File('/etc/passwd').exists()";
        System.out.println("Expression: " + expr4);
        Object result4 = eval.executeMethod(expr4);
        System.out.println("Result: " + result4);

        System.out.println("\n🔴 ATTACK 5: Environment variables");
        String expr5 = "T(java.lang.System).getenv()";
        System.out.println("Expression: " + expr5);
        System.out.println("⚠️ This would expose all environment variables!");
    }

    private static void demonstrateSafeFilter(Scanner scanner, SecureExpressionEvaluator eval) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SAFE FILTER DEMONSTRATION");
        System.out.println("=".repeat(50));

        System.out.println("\nFields: name, category, price, quantity, inStock");
        System.out.println("Operations:");
        System.out.println("  • String: equals, contains, startsWith, endsWith");
        System.out.println("  • Numeric: >, >=, <, <=, ==");
        System.out.println("  • Boolean: ==");

        System.out.print("\nEnter field: ");
        String field = scanner.nextLine().trim();

        System.out.print("Enter operation: ");
        String operation = scanner.nextLine().trim();

        System.out.print("Enter value: ");
        String value = scanner.nextLine().trim();

        List<Product> results = eval.safeFilter(operation, field, value);

        System.out.println("\nResults (" + results.size() + " products):");
        results.forEach(p -> System.out.println("  " + p));
    }

    private static void testExpressionValidation(Scanner scanner, SecureExpressionEvaluator eval) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("EXPRESSION VALIDATION TEST");
        System.out.println("=".repeat(50));

        System.out.println("\nEnter expressions to test (empty line to finish):");

        while (true) {
            System.out.print("\nExpression: ");
            String expr = scanner.nextLine().trim();

            if (expr.isEmpty()) {
                break;
            }

            try {
                List<Product> results = eval.filter(expr);
                System.out.println("✅ VALID - " + results.size() + " results");
            } catch (SecurityException e) {
                System.out.println("❌ INVALID - " + e.getMessage());
            }
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-917");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-917 VULNERABLE):");
        System.out.println("  • Direct expression evaluation without validation");
        System.out.println("  • Can access system properties");
        System.out.println("  • Can execute arbitrary code via Runtime");
        System.out.println("  • Can load arbitrary classes");
        System.out.println("  • Can access file system");
        System.out.println("  • Can read environment variables");
        System.out.println("  • No input validation or sanitization");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Input Validation:");
        System.out.println("     • Pattern whitelist");
        System.out.println("     • Dangerous pattern blocking");
        System.out.println("     • Length limits");

        System.out.println("\n  2. Restricted Context:");
        System.out.println("     • Custom MethodResolver");
        System.out.println("     • Custom PropertyAccessor");
        System.out.println("     • Disabled type lookup");
        System.out.println("     • Read-only access");

        System.out.println("\n  3. Operation Whitelist:");
        System.out.println("     • Allowed methods only");
        System.out.println("     • Allowed properties only");
        System.out.println("     • Safe operations only");

        System.out.println("\n  4. Safe Filter API:");
        System.out.println("     • Predefined operations");
        System.out.println("     • Type-safe field access");
        System.out.println("     • No expression parsing");

        System.out.println("\n📋 BEST PRACTICES FOR CWE-917:");
        System.out.println("  1. Never evaluate user input directly");
        System.out.println("  2. Use whitelist-based validation");
        System.out.println("  3. Restrict expression context");
        System.out.println("  4. Disable dangerous features (type lookup, constructor invocation)");
        System.out.println("  5. Use predefined operations instead of expressions");
        System.out.println("  6. Implement proper access controls");
        System.out.println("  7. Use security manager");
        System.out.println("  8. Consider using a safe query builder instead");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Avoid expression languages with user input");
        System.out.println("  • Use predefined filter operations");
        System.out.println("  • If expressions are required, use strict validation");
        System.out.println("  • Consider using a query builder pattern");
        System.out.println("  • Implement comprehensive logging");
    }
}