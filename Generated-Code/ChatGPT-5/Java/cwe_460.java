package chatgpt5.java;

import java.io.FileWriter;
import java.io.IOException;

/*
 * Demonstration of CWE-460
 *
 * This program opens a FileWriter to log data,
 * but does not guarantee proper cleanup if an exception occurs.
 */

public class cwe_460 {

    public static void main(String[] args) {

        FileWriter writer = null;

        try {
            // Open FileWriter in append mode
            writer = new FileWriter("application.log", true);

            writer.write("Application started.\n");

            // Simulate processing
            int result = 10 / 2;
            writer.write("Computation result: " + result + "\n");

            writer.write("Application finished successfully.\n");

        } catch (Exception e) { // ❌ Generic catch + possible resource leak
            System.out.println("An error occurred while logging.");
            e.printStackTrace();

        }

        // ❌ If exception occurs before close(), resource may not be released properly
        try {
            if (writer != null) {
                writer.close();
                System.out.println("Log file closed.");
            }
        } catch (IOException e) {
            System.out.println("Failed to close log file.");
        }
    }
}