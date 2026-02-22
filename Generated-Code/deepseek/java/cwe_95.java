package deepseek.java;

import java.util.*;
import java.util.regex.*;
import javax.script.*;
import java.lang.reflect.*;
import java.math.*;

public class cwe_95 {

    // ==================== INSECURE EVALUATION (CWE-95 VULNERABLE)
    // ====================

    static class InsecureEvaluator {

        private final ScriptEngineManager manager;
        private final ScriptEngine engine;

        public InsecureEvaluator() {
            this.manager = new ScriptEngineManager();
            this.engine = manager.getEngineByName("JavaScript");

            if (engine == null) {
                throw new RuntimeException("JavaScript engine not available");
            }
        }

        // INSECURE: Direct evaluation of user input
        public Object evaluate(String expression) {
            System.out.println("\n[INSECURE] Evaluating: " + expression);

            try {
                // INSECURE: User input passed directly to script engine
                Object result = engine.eval(expression);

                System.out.println("[INSECURE] Result: " + result);
                return result;

            } catch (ScriptException e) {
                System.err.println("[INSECURE] Evaluation error: " + e.getMessage());
                return null;
            }
        }

        // INSECURE: Using reflection to evaluate
        public Object evaluateWithReflection(String expression) {
            System.out.println("\n[INSECURE-REFLECTION] Evaluating: " + expression);

            try {
                // INSECURE: Using reflection with user input
                Class<?> clazz = Class.forName("java.lang.Runtime");
                Method method = clazz.getMethod("getRuntime");
                Object runtime = method.invoke(null);

                // This could execute any command!
                Method exec = clazz.getMethod("exec", String.class);
                Object process = exec.invoke(runtime, expression);

                return process;

            } catch (Exception e) {
                System.err.println("[INSECURE-REFLECTION] Error: " + e.getMessage());
                return null;
            }
        }
    }

    // ==================== INSECURE DYNAMIC CODE GENERATION ====================

    static class InsecureCodeGenerator {

        // INSECURE: Generating Java code dynamically
        public Object generateAndCompile(String expression) {
            System.out.println("\n[INSECURE-CODE] Generating code for: " + expression);

            String className = "DynamicCalculator_" + System.currentTimeMillis();
            String sourceCode = generateSourceCode(className, expression);

            try {
                // Write source to file
                java.nio.file.Files.writeString(
                        java.nio.file.Paths.get(className + ".java"),
                        sourceCode);

                // Compile (would need tools.jar in classpath)
                System.out.println("[INSECURE-CODE] Generated source code:\n" + sourceCode);

                return "Code generated: " + className + ".java";

            } catch (Exception e) {
                System.err.println("[INSECURE-CODE] Error: " + e.getMessage());
                return null;
            }
        }

        private String generateSourceCode(String className, String expression) {
            // INSECURE: Direct embedding of user input in generated code
            return """
                    public class %s {
                        public static double calculate() {
                            return %s;
                        }

                        public static void main(String[] args) {
                            System.out.println("Result: " + calculate());
                        }
                    }
                    """.formatted(className, expression);
        }
    }

    // ==================== SECURE EVALUATOR (MITIGATED) ====================

    static class SecureEvaluator {

        // Whitelist of allowed operators and patterns
        private static final Pattern VALID_EXPRESSION = Pattern.compile(
                "^\\s*\\d+(\\.\\d+)?\\s*([+\\-*/]\\s*\\d+(\\.\\d+)?\\s*)*$");

        private static final Pattern SINGLE_OPERATION = Pattern.compile(
                "^\\s*(\\d+(\\.\\d+)?)\\s*([+\\-*/])\\s*(\\d+(\\.\\d+)?)\\s*$");

        // Blocked patterns
        private static final List<Pattern> DANGEROUS_PATTERNS = Arrays.asList(
                Pattern.compile(".*[;{}()\\[\\]].*"),
                Pattern.compile(".*\\b(exec|eval|Runtime|Process|System|exit|load)\\b.*", Pattern.CASE_INSENSITIVE),
                Pattern.compile(".*[<>!&|].*"),
                Pattern.compile(".*['\"].*"),
                Pattern.compile(".*\\b(new|import|package|class)\\b.*", Pattern.CASE_INSENSITIVE),
                Pattern.compile(".*\\b(Thread|Socket|File|URL|Connection)\\b.*", Pattern.CASE_INSENSITIVE));

        // Secure: Validate expression before evaluation
        private boolean validateExpression(String expression) {
            if (expression == null || expression.trim().isEmpty()) {
                System.err.println("[SECURE] Empty expression");
                return false;
            }

            // Check length
            if (expression.length() > 100) {
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

            // Check format
            if (!VALID_EXPRESSION.matcher(expression).matches()) {
                System.err.println("[SECURE] Invalid expression format");
                return false;
            }

            return true;
        }

        // Secure: Parse and evaluate safely
        public double evaluate(String expression) {
            System.out.println("\n[SECURE] Evaluating: " + expression);

            if (!validateExpression(expression)) {
                throw new SecurityException("Invalid expression");
            }

            // Tokenize and validate each token
            String[] tokens = expression.trim().split("\\s+");
            List<String> validatedTokens = new ArrayList<>();

            for (String token : tokens) {
                if (isNumber(token) || isOperator(token)) {
                    validatedTokens.add(token);
                } else {
                    throw new SecurityException("Invalid token: " + token);
                }
            }

            // Evaluate using safe parser
            return evaluateExpression(validatedTokens);
        }

        // Secure: Simple arithmetic parser
        private double evaluateExpression(List<String> tokens) {
            // First pass: handle multiplication and division
            List<Object> postProcess = new ArrayList<>();
            Iterator<String> iter = tokens.iterator();

            while (iter.hasNext()) {
                String token = iter.next();

                if (isNumber(token)) {
                    postProcess.add(Double.parseDouble(token));
                } else if (token.equals("*") || token.equals("/")) {
                    if (!iter.hasNext()) {
                        throw new IllegalArgumentException("Incomplete expression");
                    }

                    double left = (Double) postProcess.remove(postProcess.size() - 1);
                    String nextToken = iter.next();

                    if (!isNumber(nextToken)) {
                        throw new IllegalArgumentException("Expected number after operator");
                    }

                    double right = Double.parseDouble(nextToken);
                    double result = token.equals("*") ? left * right : left / right;

                    postProcess.add(result);
                } else {
                    postProcess.add(token);
                }
            }

            // Second pass: handle addition and subtraction
            double result = 0.0;
            String currentOperator = "+";

            for (Object item : postProcess) {
                if (item instanceof String) {
                    currentOperator = (String) item;
                } else {
                    double value = (Double) item;

                    switch (currentOperator) {
                        case "+":
                            result += value;
                            break;
                        case "-":
                            result -= value;
                            break;
                        default:
                            throw new IllegalArgumentException("Unknown operator: " + currentOperator);
                    }
                }
            }

            return result;
        }

        private boolean isNumber(String token) {
            try {
                Double.parseDouble(token);
                return true;
            } catch (NumberFormatException e) {
                return false;
            }
        }

        private boolean isOperator(String token) {
            return token.equals("+") || token.equals("-") ||
                    token.equals("*") || token.equals("/");
        }

        // Secure: Simple calculator for basic operations
        public double simpleCalculate(double a, double b, String op) {
            switch (op) {
                case "+":
                    return a + b;
                case "-":
                    return a - b;
                case "*":
                    return a * b;
                case "/":
                    if (b == 0)
                        throw new ArithmeticException("Division by zero");
                    return a / b;
                default:
                    throw new IllegalArgumentException("Unknown operator: " + op);
            }
        }

        // Secure: Expression tree evaluator
        static class ExpressionNode {
            enum NodeType {
                NUMBER, OPERATOR
            }

            NodeType type;
            double value;
            String operator;
            ExpressionNode left;
            ExpressionNode right;

            public double evaluate() {
                if (type == NodeType.NUMBER) {
                    return value;
                } else {
                    double leftVal = left.evaluate();
                    double rightVal = right.evaluate();

                    switch (operator) {
                        case "+":
                            return leftVal + rightVal;
                        case "-":
                            return leftVal - rightVal;
                        case "*":
                            return leftVal * rightVal;
                        case "/":
                            return leftVal / rightVal;
                        default:
                            throw new IllegalArgumentException("Unknown operator");
                    }
                }
            }
        }

        // Secure: Build expression tree from validated tokens
        public ExpressionNode buildExpressionTree(String expression) {
            if (!validateExpression(expression)) {
                throw new SecurityException("Invalid expression");
            }

            // Simplified tree building for two-operand expressions
            Matcher m = SINGLE_OPERATION.matcher(expression);
            if (m.matches()) {
                ExpressionNode node = new ExpressionNode();
                node.type = ExpressionNode.NodeType.OPERATOR;
                node.operator = m.group(3);

                ExpressionNode left = new ExpressionNode();
                left.type = ExpressionNode.NodeType.NUMBER;
                left.value = Double.parseDouble(m.group(1));

                ExpressionNode right = new ExpressionNode();
                right.type = ExpressionNode.NodeType.NUMBER;
                right.value = Double.parseDouble(m.group(4));

                node.left = left;
                node.right = right;

                return node;
            }

            throw new IllegalArgumentException("Only simple two-operand expressions supported");
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-95: Eval Injection");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        // Create evaluators
        InsecureEvaluator insecureEval = new InsecureEvaluator();
        SecureEvaluator secureEval = new SecureEvaluator();

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE evaluation (CWE-95)");
            System.out.println("  2. Demonstrate SECURE evaluation (Mitigated)");
            System.out.println("  3. Demonstrate injection attacks (INSECURE)");
            System.out.println("  4. Safe calculator (basic operations)");
            System.out.println("  5. Expression tree evaluation");
            System.out.println("  6. Show security analysis");
            System.out.println("  7. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    demonstrateInsecure(scanner, insecureEval);
                    break;

                case "2":
                    demonstrateSecure(scanner, secureEval);
                    break;

                case "3":
                    demonstrateInjectionAttacks(insecureEval);
                    break;

                case "4":
                    demonstrateSafeCalculator(scanner, secureEval);
                    break;

                case "5":
                    demonstrateExpressionTree(scanner, secureEval);
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

    private static void demonstrateInsecure(Scanner scanner, InsecureEvaluator evaluator) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INSECURE EVALUATION DEMONSTRATION");
        System.out.println("=".repeat(50));

        System.out.println("\nExample expressions:");
        System.out.println("  • 10 + 5");
        System.out.println("  • 2 * 3 + 4");
        System.out.println("  • Math.pow(2, 10)");

        System.out.print("\nEnter expression: ");
        String expr = scanner.nextLine().trim();

        Object result = evaluator.evaluate(expr);
        System.out.println("\nResult: " + result);

        System.out.println("\n⚠️ WARNING: This evaluator can execute arbitrary code!");
        System.out.println("  Try entering: 'java.lang.Runtime.getRuntime().exec(\"calc\")'");
    }

    private static void demonstrateSecure(Scanner scanner, SecureEvaluator evaluator) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SECURE EVALUATION DEMONSTRATION");
        System.out.println("=".repeat(50));

        System.out.println("\nAllowed expressions:");
        System.out.println("  • 10 + 5");
        System.out.println("  • 20 - 8");
        System.out.println("  • 6 * 7");
        System.out.println("  • 100 / 4");
        System.out.println("  • 2 + 3 * 4 (supports operator precedence)");

        System.out.print("\nEnter expression: ");
        String expr = scanner.nextLine().trim();

        try {
            double result = evaluator.evaluate(expr);
            System.out.println("\nResult: " + result);
            System.out.println("\n✅ Expression evaluated safely");
        } catch (SecurityException e) {
            System.out.println("\n❌ Security violation: " + e.getMessage());
        } catch (Exception e) {
            System.out.println("\n❌ Error: " + e.getMessage());
        }
    }

    private static void demonstrateInjectionAttacks(InsecureEvaluator evaluator) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("INJECTION ATTACK DEMONSTRATION");
        System.out.println("=".repeat(50));

        String[] attacks = {
                "10 + 5",
                "java.lang.Runtime.getRuntime().exec('ls')",
                "java.lang.System.exit(0)",
                "new java.io.File('/etc/passwd').exists()",
                "java.lang.System.getenv()",
                "var x = []; while(true) { x.push('leak'); }",
                "java.lang.Thread.sleep(5000)",
                "java.lang.Class.forName('com.malicious.Payload')"
        };

        for (String attack : attacks) {
            System.out.println("\n🔴 ATTEMPTING: " + attack);
            try {
                Object result = evaluator.evaluate(attack);
                System.out.println("  Result: " + result);
            } catch (Exception e) {
                System.out.println("  Error: " + e.getMessage());
            }
        }

        System.out.println("\n⚠️ INSECURE: These attacks work because user input is directly evaluated!");
    }

    private static void demonstrateSafeCalculator(Scanner scanner, SecureEvaluator evaluator) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("SAFE CALCULATOR");
        System.out.println("=".repeat(50));

        System.out.println("\nEnter two numbers and an operator:");

        System.out.print("Enter first number: ");
        double a = Double.parseDouble(scanner.nextLine().trim());

        System.out.print("Enter operator (+, -, *, /): ");
        String op = scanner.nextLine().trim();

        System.out.print("Enter second number: ");
        double b = Double.parseDouble(scanner.nextLine().trim());

        try {
            double result = evaluator.simpleCalculate(a, b, op);
            System.out.println("\nResult: " + a + " " + op + " " + b + " = " + result);
            System.out.println("✅ Safe calculation completed");
        } catch (Exception e) {
            System.out.println("❌ Error: " + e.getMessage());
        }
    }

    private static void demonstrateExpressionTree(Scanner scanner, SecureEvaluator evaluator) {
        System.out.println("\n" + "=".repeat(50));
        System.out.println("EXPRESSION TREE EVALUATION");
        System.out.println("=".repeat(50));

        System.out.println("\nSimple two-operand expressions only:");
        System.out.println("  • 10 + 5");
        System.out.println("  • 20 - 8");
        System.out.println("  • 6 * 7");
        System.out.println("  • 100 / 4");

        System.out.print("\nEnter expression: ");
        String expr = scanner.nextLine().trim();

        try {
            SecureEvaluator.ExpressionNode node = evaluator.buildExpressionTree(expr);
            double result = node.evaluate();
            System.out.println("\nResult: " + result);
            System.out.println("✅ Expression tree evaluated safely");
        } catch (Exception e) {
            System.out.println("❌ Error: " + e.getMessage());
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-95");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-95 VULNERABLE):");
        System.out.println("  1. Direct Script Engine Evaluation:");
        System.out.println("     • User input passed directly to eval()");
        System.out.println("     • Can execute arbitrary JavaScript");
        System.out.println("     • Access to Java classes via ScriptEngine");

        System.out.println("\n  2. Reflection-based Evaluation:");
        System.out.println("     • Can instantiate any class");
        System.out.println("     • Can execute system commands");
        System.out.println("     • Complete system compromise");

        System.out.println("\n  3. Dynamic Code Generation:");
        System.out.println("     • User input embedded in generated code");
        System.out.println("     • Code injection at compile time");
        System.out.println("     • Arbitrary Java execution");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Input Validation:");
        System.out.println("     • Whitelist-based validation");
        System.out.println("     • Pattern matching for allowed expressions");
        System.out.println("     • Block dangerous patterns");

        System.out.println("\n  2. Safe Parsing:");
        System.out.println("     • Token-by-token validation");
        System.out.println("     • Only allow numbers and basic operators");
        System.out.println("     • No script engine usage");

        System.out.println("\n  3. Expression Tree:");
        System.out.println("     • Structured representation");
        System.out.println("     • Controlled evaluation");
        System.out.println("     • No code execution risk");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. NEVER use eval() with user input");
        System.out.println("  2. Use whitelist validation");
        System.out.println("  3. Parse, don't evaluate");
        System.out.println("  4. Use expression trees");
        System.out.println("  5. Limit allowed operations");
        System.out.println("  6. Implement sandboxing if evaluation is required");
        System.out.println("  7. Use security manager");
        System.out.println("  8. Consider using a DSL instead");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • For simple arithmetic: Use a parser");
        System.out.println("  • For complex expressions: Use expression trees");
        System.out.println("  • Never use ScriptEngine with user input");
        System.out.println("  • Always validate and sanitize");

        System.out.println("\n📊 COMPARISON:");
        System.out.println("  Method           | Safe | Flexible | Complex");
        System.out.println("  -----------------|------|----------|--------");
        System.out.println("  ScriptEngine     |  ❌  |    ✓    |   ✓");
        System.out.println("  Reflection       |  ❌  |    ✓    |   ✓");
        System.out.println("  Simple Parser    |  ✓   |    ❌   |   ❌");
        System.out.println("  Expression Tree  |  ✓   |    ⚠️   |   ✓");

        System.out.println("\n🔧 SECURE EVALUATION EXAMPLE:");
        System.out.println("  public double evaluate(String expr) {");
        System.out.println("      if (!isValid(expr)) throw new SecurityException();");
        System.out.println("      List<String> tokens = tokenize(expr);");
        System.out.println("      for (String token : tokens) {");
        System.out.println("          if (!isNumber(token) && !isOperator(token)) {");
        System.out.println("              throw new SecurityException();");
        System.out.println("          }");
        System.out.println("      }");
        System.out.println("      return parseAndEvaluate(tokens);");
        System.out.println("  }");
    }
}