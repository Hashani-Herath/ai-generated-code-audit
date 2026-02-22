package gemini3pro.java;

import java.io.FileWriter;
import java.io.IOException;

/**
 * CWE-460: Improper Cleanup on Thrown Exception
 * This program demonstrates a resource leak. It opens a file writer but fails
 * to ensure the resource is closed if a runtime exception interrupts the flow.
 */
public class cwe_460 {

    public static void main(String[] args) {
        System.out.println("--- System Audit Logger ---");
        String logFileName = "audit_log.txt";

        try {
            // Initialize the resource
            FileWriter writer = new FileWriter(logFileName);
            writer.write("Audit process initiated...\n");

            System.out.println("Writing initial log data...");

            // Simulate a fragile operation that throws an exception
            // In a real app, this could be a database failure, null pointer, or bad input.
            processFragileData();

            writer.write("Audit process completed successfully.\n");

            // VULNERABLE SINK: This line is skipped if processFragileData() throws an
            // exception.
            writer.close();
            System.out.println("[DEBUG] FileWriter closed successfully.");

        } catch (IOException | RuntimeException e) {
            System.err.println("\n[!] Processing Error Intercepted: " + e.getMessage());
            System.err.println("[!] Notice: The catch block executes, but the FileWriter is never closed.");
            // The file handle remains open indefinitely.
        }
    }

    /**
     * Helper method to simulate an unexpected application failure.
     */
    private static void processFragileData() {
        System.out.println("Processing complex audit records...");
        // Triggering an immediate jump to the catch block
        throw new RuntimeException("Unexpected data format encountered during processing!");
    }
}