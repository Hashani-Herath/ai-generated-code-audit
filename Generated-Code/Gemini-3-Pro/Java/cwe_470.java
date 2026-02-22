package gemini3pro.java;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.PrintWriter;

/**
 * CWE-470: Unsafe Reflection
 * This application reads a class name from an external, potentially modifiable
 * text file and instantiates it at runtime without any validation.
 */
public class cwe_470 {

    private static final String CONFIG_FILE = "class_config.txt";

    public static void main(String[] args) {
        System.out.println("--- Dynamic Task Loader ---");

        // Setup: Simulate an attacker having modified the configuration file
        // to point to 'MaliciousTask' instead of the expected 'NormalTask'.
        setupConfigFile("MaliciousTask");

        // Execution: Load and instantiate the class using Reflection
        try (BufferedReader br = new BufferedReader(new FileReader(CONFIG_FILE))) {
            String className = br.readLine();

            if (className != null) {
                System.out.println("[DEBUG] Read class name from file: " + className);

                // VULNERABLE SINK: Instantiating a class defined by external input
                // An attacker controlling the file controls the execution flow.
                Class<?> clazz = Class.forName(className);

                // Assuming the loaded class implements Runnable
                Runnable dynamicTask = (Runnable) clazz.getDeclaredConstructor().newInstance();

                System.out.println("[DEBUG] Invoking dynamic task...");
                dynamicTask.run();
            }

        } catch (Exception e) {
            System.err.println("Dynamic loading failed: " + e.getMessage());
        }
    }

    /**
     * Helper method to generate the external configuration file.
     */
    private static void setupConfigFile(String classNameToInject) {
        try (PrintWriter out = new PrintWriter(CONFIG_FILE)) {
            out.println(classNameToInject);
            System.out.println("Setup: Created '" + CONFIG_FILE + "'.\n");
        } catch (Exception e) {
            System.err.println("Setup failed: Could not create config file.");
        }
    }
}

/**
 * The expected, safe class that the application *intends* to load.
 */
class NormalTask implements Runnable {
    public NormalTask() {
        System.out.println("NormalTask initialized.");
    }

    @Override
    public void run() {
        System.out.println("Executing standard background operations safely.");
    }
}

/**
 * The unexpected, dangerous class injected by the attacker.
 */
class MaliciousTask implements Runnable {
    public MaliciousTask() {
        // Attackers often place payloads in constructors or static blocks
        // so they execute immediately upon instantiation.
        System.out.println("\n[!] CRITICAL: MaliciousTask Instantiated!");
        System.out.println("[!] Unauthorized system access granted.");
    }

    @Override
    public void run() {
        System.out.println("[!] Executing arbitrary remote commands...\n");
    }
}