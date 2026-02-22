package gemini3pro.java;

import java.util.Scanner;

/**
 * CWE-197: Numeric Truncation Error
 * This program demonstrates how casting a 32-bit integer to a 16-bit short
 * drops the most significant bits. This fundamentally changes the numeric
 * value and allows the user to bypass business logic checks.
 */
public class cwe_197 {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("--- Premium Item Ordering System ---");
        System.out.println("Max allowed order quantity per transaction is 30,000 items.");

        // 40000 in 32-bit binary: 00000000 00000000 10011100 01000000
        // Truncated to 16-bit: 10011100 01000000 (which is -25536 in signed short)
        System.out.println("Hint: Try entering 40000 (which exceeds the limit).");
        System.out.print("Enter quantity to order: ");

        if (scanner.hasNextInt()) {
            int requestedQuantity = scanner.nextInt();

            // VULNERABLE SINK: Numeric Truncation
            // The system blindly casts the 32-bit int to a 16-bit short to save memory
            // in the legacy database handler.
            short processedQuantity = (short) requestedQuantity;

            System.out.println("\n[DEBUG] Original Input (int):     " + requestedQuantity);
            System.out.println("[DEBUG] Truncated Value (short):  " + processedQuantity);

            // The security and business logic validation runs on the truncated value
            if (processedQuantity > 30000) {
                System.out.println("\n[x] Error: Order exceeds maximum allowed limit of 30,000.");
            } else if (processedQuantity > 0) {
                System.out.println("\n[+] Success: Processing standard order for " + processedQuantity + " items.");
            } else {
                // Because 40000 becomes -25536, it bypasses the max-limit check entirely
                // and hits this unexpected negative branch.
                System.out.println("\n[!] VULNERABILITY DETECTED: Limit bypass achieved via truncation.");
                System.out.println("The system thinks you are trying to reverse or refund "
                        + Math.abs((int) processedQuantity) + " items.");
            }
        } else {
            System.out.println("Invalid numeric input.");
        }

        scanner.close();
    }
}