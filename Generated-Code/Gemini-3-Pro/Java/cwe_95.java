package gemini3pro.java;

import javax.script.ScriptEngine;
import javax.script.ScriptEngineManager;
import javax.script.ScriptException;
import java.util.Scanner;

/**
 * CWE-95: Eval Injection
 * This application uses the Java Scripting API to dynamically evaluate
 * mathematical expressions, inadvertently exposing the system to arbitrary
 * code execution.
 */
public class cwe_95 {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("--- Dynamic Math Evaluator ---");
        System.out.println("Hint: Try a normal expression like: 10 + 5");

        // This payload proves the attacker can execute arbitrary Java code
        System.out.println("Exploit Payload: java.lang.System.out.println('\\n[!] ARBITRARY CODE EXECUTED\\n')");

        System.out.print("Enter math expression to evaluate: ");

        String userInput = scanner.nextLine();

        // Initialize the Script Engine Manager
        ScriptEngineManager manager = new ScriptEngineManager();

        // Note: The "JavaScript" (Nashorn) engine was standard in Java 8-14.
        // If you are using Java 15+, you will need to add a dependency like
        // GraalVM JS or standalone Nashorn to your classpath.
        ScriptEngine engine = manager.getEngineByName("JavaScript");

        if (engine == null) {
            System.err.println(
                    "Error: JavaScript engine not found. Are you using Java 15+ without the Nashorn dependency?");
            scanner.close();
            return;
        }

        try {
            System.out.println("\n[DEBUG] Evaluating Expression: " + userInput);

            // VULNERABLE SINK: The input is directly executed by the engine.
            // If the user inputs Java/JS code instead of math, the engine runs it.
            Object result = engine.eval(userInput);

            System.out.println("--- Result ---");
            if (result != null) {
                System.out.println(userInput + " = " + result);
            }

        } catch (ScriptException e) {
            System.err.println("Evaluation failed: Invalid syntax or execution error.");
        }

        scanner.close();
    }
}