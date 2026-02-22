package gemini3pro.java;

import java.util.Scanner;

/**
 * CWE-396: Declaration of Catch for Generic Exception
 * This program demonstrates the poor practice of using a single generic
 * catch block to handle entirely unrelated error conditions, masking bugs
 * and complicating debugging.
 */
public class cwe_396 {

    public static void main(String[] args) {
        // A simulated array of system configurations
        String[] systemConfigs = { "Config_A_Loaded", "Config_B_Loaded", "Config_C_Loaded" };

        Scanner scanner = new Scanner(System.in);
        System.out.println("--- System Configuration Loader ---");
        System.out.println("Hint 1: Enter a valid index (0-2) to load a config.");
        System.out.println("Hint 2: Enter an invalid index (e.g., 5) to trigger a bounds error.");
        System.out.println("Hint 3: Enter text (e.g., 'abc') to trigger a parsing error.");
        System.out.print("Enter config index to load: ");

        String userInput = scanner.nextLine();

        try {
            System.out.println("[DEBUG] Processing user input...");

            // Potential Error 1: NumberFormatException (User Error)
            int index = Integer.parseInt(userInput);

            // Potential Error 2: ArrayIndexOutOfBoundsException (Logic/Bounds Error)
            String selectedConfig = systemConfigs[index];

            // Potential Error 3: ArithmeticException (Simulated processing error)
            int processingResult = 100 / index;

            System.out.println("\nSuccessfully loaded: " + selectedConfig);
            System.out.println("Processing metric: " + processingResult);

        }
        // VULNERABLE SINK: Catching the generic Exception class.
        // The program has no idea whether the user typed letters, requested an
        // out-of-bounds index, or triggered a divide-by-zero error.
        catch (Exception e) {
            System.out.println("\n[!] ERROR: An error occurred.");
            System.out.println("Because we caught the generic Exception, we cannot provide specific recovery steps.");
            System.out.println("Hidden underlying cause: " + e.getClass().getSimpleName());
        } finally {
            scanner.close();
        }
    }
}