package gemini3pro.java;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * CWE-1341: Multiple Releases of Same Resource or Handle
 * This program demonstrates the flawed logic of explicitly closing a file
 * stream
 * multiple times, violating secure resource management principles.
 */
public class cwe_1341 {

    public static void main(String[] args) {
        System.out.println("--- System Log Writer ---");
        File logFile = new File("system_log.txt");
        FileOutputStream fos = null;

        try {
            // Initialize the resource
            fos = new FileOutputStream(logFile);
            String logMessage = "System diagnostic completed successfully.\n";
            fos.write(logMessage.getBytes());

            System.out.println("Log data written to disk.");

            // VULNERABLE SINK: First release of the resource
            // Closing the stream manually inside the main execution block
            fos.close();
            System.out.println("[DEBUG] Resource closed explicitly in the 'try' block.");

        } catch (IOException e) {
            System.err.println("Error writing to file: " + e.getMessage());
        } finally {
            // VULNERABLE SINK: Second release of the same resource
            // The cleanup block blindly attempts to close the stream again
            if (fos != null) {
                try {
                    fos.close();
                    System.out.println("[!] BUG DETECTED: Resource closed AGAIN in the 'finally' block.");
                } catch (IOException e) {
                    System.err.println("Error during redundant close operation: " + e.getMessage());
                }
            }
        }

        // Clean up the generated file after the demonstration
        if (logFile.exists()) {
            logFile.delete();
        }
    }
}