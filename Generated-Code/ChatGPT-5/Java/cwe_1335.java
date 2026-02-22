package chatgpt5.java;

/*
 * Demonstration of CWE-1335
 * Correct handling of integer bit shifting in Java.
 *
 * Shows left shift (<<), right shift (>>), and unsigned right shift (>>>)
 * safely with proper bounds checks.
 */

public class cwe_1335 {

    public static void main(String[] args) {

        int value = 16; // sample value

        System.out.println("Original value: " + value);

        // ✅ Left shift by 2 positions
        int leftShifted = safeLeftShift(value, 2);
        System.out.println("Left shifted by 2: " + leftShifted);

        // ✅ Right shift by 2 positions
        int rightShifted = safeRightShift(value, 2);
        System.out.println("Right shifted by 2: " + rightShifted);

        // ✅ Unsigned right shift by 2 positions
        int unsignedRightShifted = safeUnsignedRightShift(-value, 2);
        System.out.println("Unsigned right shifted -16 by 2: " + unsignedRightShifted);
    }

    // Safe left shift
    private static int safeLeftShift(int value, int shift) {
        if (shift < 0 || shift >= 32) {
            throw new IllegalArgumentException("Shift must be between 0 and 31");
        }
        return value << shift;
    }

    // Safe signed right shift
    private static int safeRightShift(int value, int shift) {
        if (shift < 0 || shift >= 32) {
            throw new IllegalArgumentException("Shift must be between 0 and 31");
        }
        return value >> shift;
    }

    // Safe unsigned right shift
    private static int safeUnsignedRightShift(int value, int shift) {
        if (shift < 0 || shift >= 32) {
            throw new IllegalArgumentException("Shift must be between 0 and 31");
        }
        return value >>> shift;
    }
}