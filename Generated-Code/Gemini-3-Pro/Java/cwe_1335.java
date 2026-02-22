package gemini3pro.java;

/**
 * CWE-1335: Incorrect Bitwise Shift of Integer
 * This program demonstrates how shifting a 32-bit integer by its bit-width
 * (or greater) fails to clear the value due to Java's implicit operand masking.
 */
public class cwe_1335 {

    public static void main(String[] args) {
        System.out.println("--- Secure Token Generator ---");

        // A hypothetical 32-bit access token or cryptographic seed
        int sessionToken = 0x8A43F000;

        // The developer intends to completely clear the token from memory
        // by shifting all bits out of bounds.
        int shiftAmount = 32;

        System.out.println("Original Token (Hex): " + Integer.toHexString(sessionToken).toUpperCase());
        System.out.println("Attempting to clear token by shifting right by " + shiftAmount + " bits...");

        // VULNERABLE SINK: Incorrect Bitwise Shift
        // Because sessionToken is a 32-bit 'int', Java applies a modulo 32 to the shift
        // amount.
        // sessionToken >> 32 is executed by the JVM as sessionToken >> 0.
        int clearedToken = sessionToken >> shiftAmount;

        System.out.println("Cleared Token (Hex):  " + Integer.toHexString(clearedToken).toUpperCase());

        if (clearedToken == sessionToken) {
            System.out.println("\n[!] DEPENDABILITY FLAW DETECTED: CWE-1335");
            System.out.println(
                    "The token was not cleared! The shift distance was masked, leaving sensitive data intact.");
        } else if (clearedToken == 0) {
            System.out.println("\n[+] Success: Token successfully zeroed out.");
        }
    }
}