package deepseek.java;

import java.util.*;
import java.nio.*;
import java.nio.charset.*;

public class cwe_1335 {

    // ==================== INSECURE BIT SHIFTING (CWE-1335 VULNERABLE)
    // ====================

    static class InsecureBitOperations {

        // INSECURE: No overflow checking in bit shifts
        public int shiftLeftInsecure(int value, int positions) {
            System.out.println("\n[INSECURE] Left shift: " + value + " << " + positions);
            // INSECURE: No check for positions >= 32 (only lower 5 bits used)
            int result = value << positions;
            System.out.println("  Result: " + result + " (0x" + Integer.toHexString(result) + ")");
            return result;
        }

        public int shiftRightInsecure(int value, int positions) {
            System.out.println("\n[INSECURE] Right shift: " + value + " >> " + positions);
            // INSECURE: Sign extension may not be intended
            int result = value >> positions;
            System.out.println("  Result: " + result + " (0x" + Integer.toHexString(result) + ")");
            return result;
        }

        // INSECURE: Using shift for multiplication without overflow check
        public int multiplyByPowerOfTwo(int value, int power) {
            System.out.println("\n[INSECURE] Multiply " + value + " by 2^" + power);
            // INSECURE: Can overflow silently
            int result = value << power;
            System.out.println("  Result: " + result);
            System.out.println("  Expected: " + (value * Math.pow(2, power)));
            return result;
        }

        // INSECURE: Incorrect assumption about shift behavior
        public long extractBitsInsecure(long value, int start, int length) {
            System.out.println("\n[INSECURE] Extract bits " + start + "-" + (start + length));
            // INSECURE: Doesn't handle start+length > 64
            long mask = (1 << length) - 1; // Should be 1L << length
            long result = (value >> start) & mask;
            System.out.println("  Result: 0x" + Long.toHexString(result));
            return result;
        }

        // INSECURE: Packing bytes into integer with wrong shifts
        public int packBytesInsecure(byte b3, byte b2, byte b1, byte b0) {
            System.out.println("\n[INSECURE] Packing bytes: " +
                    String.format("0x%02X 0x%02X 0x%02X 0x%02X", b3, b2, b1, b0));
            // INSECURE: Sign extension issues
            int result = (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
            System.out.println("  Result: 0x" + Integer.toHexString(result));
            return result;
        }

        // INSECURE: Unpacking with wrong shifts
        public byte[] unpackBytesInsecure(int value) {
            System.out.println("\n[INSECURE] Unpacking int: 0x" + Integer.toHexString(value));
            // INSECURE: Sign extension during cast
            byte[] bytes = new byte[4];
            bytes[0] = (byte) (value >> 24);
            bytes[1] = (byte) (value >> 16);
            bytes[2] = (byte) (value >> 8);
            bytes[3] = (byte) value;

            System.out.print("  Bytes: ");
            for (byte b : bytes) {
                System.out.print(String.format("0x%02X ", b));
            }
            System.out.println();
            return bytes;
        }

        // INSECURE: Color component extraction
        public int getRedInsecure(int rgb) {
            // INSECURE: Assuming 8-bit components, no masking
            return rgb >> 16;
        }

        public int getGreenInsecure(int rgb) {
            return rgb >> 8;
        }

        public int getBlueInsecure(int rgb) {
            return rgb;
        }

        // INSECURE: Flag checking with shift
        public boolean hasFlagInsecure(int flags, int flagPosition) {
            // INSECURE: flagPosition could be >= 32
            return (flags & (1 << flagPosition)) != 0;
        }

        // INSECURE: Setting flags
        public int setFlagInsecure(int flags, int flagPosition) {
            // INSECURE: No bounds checking
            return flags | (1 << flagPosition);
        }

        // INSECURE: Arithmetic vs logical shift confusion
        public int divideByPowerOfTwoInsecure(int value, int power) {
            System.out.println("\n[INSECURE] Divide " + value + " by 2^" + power);
            // INSECURE: Using >> for negative numbers gives floor, not truncation
            int result = value >> power;
            System.out.println("  Result: " + result);
            System.out.println("  Expected (division): " + (value / (int) Math.pow(2, power)));
            return result;
        }

        // INSECURE: Rotate left implementation
        public int rotateLeftInsecure(int value, int distance) {
            System.out.println("\n[INSECURE] Rotate left " + value + " by " + distance);
            // INSECURE: Doesn't handle distance properly
            return (value << distance) | (value >>> (32 - distance));
        }
    }

    // ==================== SECURE BIT SHIFTING (CWE-1335 MITIGATED)
    // ====================

    static class SecureBitOperations {

        // SECURE: Check shift distance
        public int shiftLeftSecure(int value, int positions) {
            System.out.println("\n[SECURE] Left shift: " + value + " << " + positions);

            if (positions < 0) {
                throw new IllegalArgumentException("Shift distance cannot be negative");
            }

            if (positions >= 32) {
                // For int, only lower 5 bits are used, but we should validate
                System.out.println("  Warning: Shift distance >= 32, result will be 0");
                return 0;
            }

            // Check for overflow
            if (value != 0 && Integer.numberOfLeadingZeros(value) < positions) {
                System.out.println("  Warning: Left shift will overflow");
            }

            int result = value << positions;
            System.out.println("  Result: " + result + " (0x" + Integer.toHexString(result) + ")");
            return result;
        }

        // SECURE: Logical vs arithmetic shift choice
        public int shiftRightSecure(int value, int positions, boolean logical) {
            System.out.println("\n[SECURE] " + (logical ? "Logical" : "Arithmetic") +
                    " right shift: " + value + " >> " + positions);

            if (positions < 0) {
                throw new IllegalArgumentException("Shift distance cannot be negative");
            }

            if (positions >= 32) {
                return logical ? 0 : (value < 0 ? -1 : 0);
            }

            int result = logical ? (value >>> positions) : (value >> positions);
            System.out.println("  Result: " + result + " (0x" + Integer.toHexString(result) + ")");
            return result;
        }

        // SECURE: Multiplication with overflow check
        public int multiplyByPowerOfTwoSecure(int value, int power) {
            System.out.println("\n[SECURE] Multiply " + value + " by 2^" + power);

            if (power < 0) {
                throw new IllegalArgumentException("Power cannot be negative");
            }

            try {
                int result = Math.multiplyExact(value, 1 << power);
                System.out.println("  Result: " + result);
                return result;
            } catch (ArithmeticException e) {
                System.out.println("  Overflow detected: " + e.getMessage());
                throw e;
            }
        }

        // SECURE: Extract bits with bounds checking
        public long extractBitsSecure(long value, int start, int length) {
            System.out.println("\n[SECURE] Extract bits " + start + "-" + (start + length));

            if (start < 0 || length < 0 || start + length > 64) {
                throw new IllegalArgumentException("Invalid bit range");
            }

            if (length == 64) {
                return value;
            }

            long mask = (length == 64) ? -1L : ((1L << length) - 1);
            long result = (value >>> start) & mask;
            System.out.println("  Result: 0x" + Long.toHexString(result));
            return result;
        }

        // SECURE: Pack bytes with proper masking
        public int packBytesSecure(byte b3, byte b2, byte b1, byte b0) {
            System.out.println("\n[SECURE] Packing bytes: " +
                    String.format("0x%02X 0x%02X 0x%02X 0x%02X", b3, b2, b1, b0));

            // SECURE: Mask bytes to unsigned before shifting
            int result = ((b3 & 0xFF) << 24) |
                    ((b2 & 0xFF) << 16) |
                    ((b1 & 0xFF) << 8) |
                    (b0 & 0xFF);

            System.out.println("  Result: 0x" + Integer.toHexString(result));
            return result;
        }

        // SECURE: Unpack bytes with proper masking
        public byte[] unpackBytesSecure(int value) {
            System.out.println("\n[SECURE] Unpacking int: 0x" + Integer.toHexString(value));

            byte[] bytes = new byte[4];
            bytes[0] = (byte) ((value >>> 24) & 0xFF);
            bytes[1] = (byte) ((value >>> 16) & 0xFF);
            bytes[2] = (byte) ((value >>> 8) & 0xFF);
            bytes[3] = (byte) (value & 0xFF);

            System.out.print("  Bytes: ");
            for (byte b : bytes) {
                System.out.print(String.format("0x%02X ", b & 0xFF));
            }
            System.out.println();
            return bytes;
        }

        // SECURE: Color component extraction with masking
        public int getRedSecure(int rgb) {
            return (rgb >>> 16) & 0xFF;
        }

        public int getGreenSecure(int rgb) {
            return (rgb >>> 8) & 0xFF;
        }

        public int getBlueSecure(int rgb) {
            return rgb & 0xFF;
        }

        public int getAlphaSecure(int rgb) {
            return (rgb >>> 24) & 0xFF;
        }

        // SECURE: RGB packing
        public int packRgbSecure(int r, int g, int b) {
            if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
                throw new IllegalArgumentException("RGB values must be 0-255");
            }
            return (r << 16) | (g << 8) | b;
        }

        public int packArgbSecure(int a, int r, int g, int b) {
            if (a < 0 || a > 255 || r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
                throw new IllegalArgumentException("ARGB values must be 0-255");
            }
            return (a << 24) | (r << 16) | (g << 8) | b;
        }

        // SECURE: Flag operations with bounds checking
        public boolean hasFlagSecure(int flags, int flagPosition) {
            if (flagPosition < 0 || flagPosition >= 32) {
                throw new IllegalArgumentException("Flag position must be 0-31");
            }
            return (flags & (1 << flagPosition)) != 0;
        }

        public int setFlagSecure(int flags, int flagPosition) {
            if (flagPosition < 0 || flagPosition >= 32) {
                throw new IllegalArgumentException("Flag position must be 0-31");
            }
            return flags | (1 << flagPosition);
        }

        public int clearFlagSecure(int flags, int flagPosition) {
            if (flagPosition < 0 || flagPosition >= 32) {
                throw new IllegalArgumentException("Flag position must be 0-31");
            }
            return flags & ~(1 << flagPosition);
        }

        // SECURE: Division by power of two using logical shift for unsigned
        public int divideUnsignedByPowerOfTwo(int value, int power) {
            if (power < 0 || power >= 32) {
                throw new IllegalArgumentException("Invalid power");
            }
            return value >>> power;
        }

        // SECURE: Division by power of two using arithmetic shift (same as division)
        public int divideSignedByPowerOfTwo(int value, int power) {
            if (power < 0 || power >= 32) {
                throw new IllegalArgumentException("Invalid power");
            }
            return value >> power;
        }

        // SECURE: Rotate left with proper handling
        public int rotateLeftSecure(int value, int distance) {
            System.out.println("\n[SECURE] Rotate left " + value + " by " + distance);

            // Normalize distance
            distance = distance & 0x1F; // Only use lower 5 bits

            int result = (value << distance) | (value >>> (32 - distance));
            System.out.println("  Result: 0x" + Integer.toHexString(result));
            return result;
        }

        // SECURE: Rotate right
        public int rotateRightSecure(int value, int distance) {
            System.out.println("\n[SECURE] Rotate right " + value + " by " + distance);

            distance = distance & 0x1F;
            int result = (value >>> distance) | (value << (32 - distance));
            System.out.println("  Result: 0x" + Integer.toHexString(result));
            return result;
        }

        // SECURE: Endianness conversion
        public int swapEndianness(int value) {
            System.out.println("\n[SECURE] Swap endianness of 0x" + Integer.toHexString(value));

            int result = ((value >>> 24) & 0xFF) |
                    ((value >>> 8) & 0xFF00) |
                    ((value & 0xFF00) << 8) |
                    ((value & 0xFF) << 24);

            System.out.println("  Result: 0x" + Integer.toHexString(result));
            return result;
        }

        // SECURE: Bit field manipulation
        public int setBitField(int value, int start, int length, int fieldValue) {
            System.out.println("\n[SECURE] Set bit field at " + start + "-" + (start + length));

            if (start < 0 || length < 0 || start + length > 32) {
                throw new IllegalArgumentException("Invalid bit range");
            }

            // Create mask
            int mask = (length == 32) ? -1 : ((1 << length) - 1);

            // Shift field value to position
            int shiftedField = (fieldValue & mask) << start;

            // Clear the field in original value and insert new value
            int result = (value & ~(mask << start)) | shiftedField;

            System.out.println("  Result: 0x" + Integer.toHexString(result));
            return result;
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class BitShiftDemonstrator {

        public static void demonstrateInsecureShifts() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE BIT SHIFTING (CWE-1335)");
            System.out.println("=".repeat(60));

            InsecureBitOperations insecure = new InsecureBitOperations();

            // Test 1: Left shift beyond bit width
            System.out.println("\n🔴 Test 1: Left shift beyond bit width");
            insecure.shiftLeftInsecure(1, 32); // Result: 1 (only lower 5 bits used)
            insecure.shiftLeftInsecure(1, 33); // Result: 2 (33 & 0x1F = 1)

            // Test 2: Multiplication overflow
            System.out.println("\n🔴 Test 2: Multiplication overflow");
            insecure.multiplyByPowerOfTwo(0x40000000, 2); // Should overflow

            // Test 3: Extract bits with overflow
            System.out.println("\n🔴 Test 3: Extract bits beyond 64");
            insecure.extractBitsInsecure(0xFFFFFFFFFFFFFFFFL, 60, 10);

            // Test 4: Pack bytes with sign extension
            System.out.println("\n🔴 Test 4: Pack bytes with sign extension");
            insecure.packBytesInsecure((byte) 0xFF, (byte) 0x80, (byte) 0x7F, (byte) 0x00);

            // Test 5: Unpack with sign extension
            System.out.println("\n🔴 Test 5: Unpack with sign extension");
            insecure.unpackBytesInsecure(0x80808080);

            // Test 6: Color extraction without masking
            System.out.println("\n🔴 Test 6: Color extraction without masking");
            int color = 0xFF7F3F1F;
            System.out.println("  Color: 0x" + Integer.toHexString(color));
            System.out.println("  Red (insecure): " + insecure.getRedInsecure(color));
            System.out.println("  Green (insecure): " + insecure.getGreenInsecure(color));
            System.out.println("  Blue (insecure): " + insecure.getBlueInsecure(color));

            // Test 7: Flag operations beyond range
            System.out.println("\n🔴 Test 7: Flag operations beyond range");
            int flags = 0;
            flags = insecure.setFlagInsecure(flags, 32);
            System.out.println("  Has flag 32? " + insecure.hasFlagInsecure(flags, 32));

            // Test 8: Division vs shift for negative numbers
            System.out.println("\n🔴 Test 8: Division vs shift for negative numbers");
            insecure.divideByPowerOfTwoInsecure(-10, 2);

            // Test 9: Rotate with incorrect distance
            System.out.println("\n🔴 Test 9: Rotate with incorrect distance");
            insecure.rotateLeftInsecure(0x80000001, 33);
        }

        public static void demonstrateSecureShifts() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE BIT SHIFTING (MITIGATED)");
            System.out.println("=".repeat(60));

            SecureBitOperations secure = new SecureBitOperations();

            // Test 1: Left shift with validation
            System.out.println("\n✅ Test 1: Left shift with validation");
            try {
                secure.shiftLeftSecure(1, 32);
            } catch (IllegalArgumentException e) {
                System.out.println("  Caught: " + e.getMessage());
            }

            // Test 2: Safe multiplication
            System.out.println("\n✅ Test 2: Safe multiplication");
            try {
                secure.multiplyByPowerOfTwoSecure(0x40000000, 2);
            } catch (ArithmeticException e) {
                System.out.println("  Caught overflow: " + e.getMessage());
            }

            // Test 3: Safe bit extraction
            System.out.println("\n✅ Test 3: Safe bit extraction");
            try {
                secure.extractBitsSecure(0xFFFFFFFFFFFFFFFFL, 60, 10);
            } catch (IllegalArgumentException e) {
                System.out.println("  Caught: " + e.getMessage());
            }

            // Test 4: Safe byte packing
            System.out.println("\n✅ Test 4: Safe byte packing");
            int packed = secure.packBytesSecure((byte) 0xFF, (byte) 0x80, (byte) 0x7F, (byte) 0x00);
            secure.unpackBytesSecure(packed);

            // Test 5: Safe color extraction
            System.out.println("\n✅ Test 5: Safe color extraction");
            int color = 0xFF7F3F1F;
            System.out.println("  Color: 0x" + Integer.toHexString(color));
            System.out.println("  Alpha: " + secure.getAlphaSecure(color));
            System.out.println("  Red: " + secure.getRedSecure(color));
            System.out.println("  Green: " + secure.getGreenSecure(color));
            System.out.println("  Blue: " + secure.getBlueSecure(color));

            // Test 6: Safe flag operations
            System.out.println("\n✅ Test 6: Safe flag operations");
            int flags = 0;
            try {
                flags = secure.setFlagSecure(flags, 5);
                flags = secure.setFlagSecure(flags, 10);
                System.out.println("  Flags: 0x" + Integer.toHexString(flags));
                System.out.println("  Has flag 5? " + secure.hasFlagSecure(flags, 5));
                System.out.println("  Has flag 10? " + secure.hasFlagSecure(flags, 10));

                flags = secure.clearFlagSecure(flags, 5);
                System.out.println("  After clearing flag 5: 0x" + Integer.toHexString(flags));

            } catch (IllegalArgumentException e) {
                System.out.println("  Caught: " + e.getMessage());
            }

            // Test 7: Division vs shift comparison
            System.out.println("\n✅ Test 7: Division vs shift comparison");
            int value = -10;
            System.out.println("  Value: " + value);
            System.out.println("  Arithmetic shift (>> 2): " + secure.divideSignedByPowerOfTwo(value, 2));
            System.out.println("  Division (/4): " + (value / 4));
            System.out.println("  Logical shift (>>> 2): " + secure.divideUnsignedByPowerOfTwo(value, 2));

            // Test 8: Rotation
            System.out.println("\n✅ Test 8: Rotation");
            secure.rotateLeftSecure(0x80000001, 1);
            secure.rotateRightSecure(0x80000001, 1);

            // Test 9: Endianness conversion
            System.out.println("\n✅ Test 9: Endianness conversion");
            secure.swapEndianness(0x12345678);

            // Test 10: Bit field manipulation
            System.out.println("\n✅ Test 10: Bit field manipulation");
            int fieldResult = secure.setBitField(0xFFFF0000, 8, 8, 0x55);
            System.out.println("  Result: 0x" + Integer.toHexString(fieldResult));
        }

        public static void demonstrateEdgeCases() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("EDGE CASES DEMONSTRATION");
            System.out.println("=".repeat(60));

            SecureBitOperations secure = new SecureBitOperations();

            System.out.println("\n📊 Left Shift Edge Cases:");
            System.out.println("  1 << 31 = 0x" + Integer.toHexString(1 << 31));
            System.out.println("  1 << 32 = 0x" + Integer.toHexString(1 << 32)); // Actually 1 << 0
            System.out.println("  1L << 63 = 0x" + Long.toHexString(1L << 63));

            System.out.println("\n📊 Right Shift Edge Cases:");
            System.out.println("  -1 >> 1 = " + (-1 >> 1));
            System.out.println("  -1 >>> 1 = " + (-1 >>> 1));

            System.out.println("\n📊 Mask Generation:");
            System.out.println("  (1 << 8) - 1 = 0x" + Integer.toHexString((1 << 8) - 1));
            System.out.println("  (1L << 48) - 1 = 0x" + Long.toHexString((1L << 48) - 1));
            System.out.println("  -1L = 0x" + Long.toHexString(-1L));

            System.out.println("\n📊 Sign Extension:");
            byte b = (byte) 0xFF;
            System.out.println("  byte 0xFF as int: " + (int) b);
            System.out.println("  byte 0xFF masked: " + (b & 0xFF));
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-1335: Incorrect Bit Shifting");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE bit shifting (CWE-1335)");
            System.out.println("  2. Demonstrate SECURE bit shifting (Mitigated)");
            System.out.println("  3. Demonstrate edge cases");
            System.out.println("  4. Interactive bit shifting test");
            System.out.println("  5. Show security analysis");
            System.out.println("  6. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    BitShiftDemonstrator.demonstrateInsecureShifts();
                    break;

                case "2":
                    BitShiftDemonstrator.demonstrateSecureShifts();
                    break;

                case "3":
                    BitShiftDemonstrator.demonstrateEdgeCases();
                    break;

                case "4":
                    interactiveTest(scanner);
                    break;

                case "5":
                    showSecurityAnalysis();
                    break;

                case "6":
                    System.out.println("\nExiting...");
                    scanner.close();
                    return;

                default:
                    System.out.println("[!] Invalid option");
            }
        }
    }

    private static void interactiveTest(Scanner scanner) {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("INTERACTIVE BIT SHIFTING TEST");
        System.out.println("=".repeat(60));

        SecureBitOperations secure = new SecureBitOperations();

        System.out.print("\nEnter integer value: ");
        int value = Integer.parseInt(scanner.nextLine().trim());

        System.out.print("Enter shift distance: ");
        int distance = Integer.parseInt(scanner.nextLine().trim());

        System.out.println("\nResults:");
        System.out.println("  Value: " + value + " (0x" + Integer.toHexString(value) + ")");

        try {
            System.out.println("  Left shift << " + distance + ": " +
                    secure.shiftLeftSecure(value, distance));
        } catch (IllegalArgumentException e) {
            System.out.println("  Left shift: " + e.getMessage());
        }

        try {
            System.out.println("  Arithmetic right shift >> " + distance + ": " +
                    secure.shiftRightSecure(value, distance, false));
        } catch (IllegalArgumentException e) {
            System.out.println("  Arithmetic right shift: " + e.getMessage());
        }

        try {
            System.out.println("  Logical right shift >>> " + distance + ": " +
                    secure.shiftRightSecure(value, distance, true));
        } catch (IllegalArgumentException e) {
            System.out.println("  Logical right shift: " + e.getMessage());
        }

        System.out.println("\nRotation:");
        System.out.println("  Rotate left by " + distance + ": 0x" +
                Integer.toHexString(secure.rotateLeftSecure(value, distance)));
        System.out.println("  Rotate right by " + distance + ": 0x" +
                Integer.toHexString(secure.rotateRightSecure(value, distance)));
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-1335");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-1335 VULNERABLE):");
        System.out.println("  1. No Shift Distance Validation:");
        System.out.println("     • Only lower 5 bits of distance used for int");
        System.out.println("     • Only lower 6 bits used for long");
        System.out.println("     • Can lead to unexpected results");

        System.out.println("\n  2. Sign Extension Issues:");
        System.out.println("     • byte to int conversion without masking");
        System.out.println("     • >> vs >>> confusion");
        System.out.println("     • Negative number handling");

        System.out.println("\n  3. Overflow Ignorance:");
        System.out.println("     • Left shift can overflow silently");
        System.out.println("     • No overflow checking");
        System.out.println("     • Data corruption");

        System.out.println("\n  4. Incorrect Assumptions:");
        System.out.println("     • Assuming 1 << length works for length=32");
        System.out.println("     • Not handling edge cases");
        System.out.println("     • Endianness confusion");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Distance Validation:");
        System.out.println("     • Check shift distance range");
        System.out.println("     • Handle edge cases explicitly");
        System.out.println("     • Document behavior");

        System.out.println("\n  2. Proper Masking:");
        System.out.println("     • Always mask bytes with 0xFF");
        System.out.println("     • Use long literals (1L) for 64-bit");
        System.out.println("     • Handle sign extension");

        System.out.println("\n  3. Overflow Detection:");
        System.out.println("     • Check for overflow before shift");
        System.out.println("     • Use Math.multiplyExact for multiplication");
        System.out.println("     • Validate results");

        System.out.println("\n  4. Clear Intent:");
        System.out.println("     • Use >>> for logical shift");
        System.out.println("     • Use >> for arithmetic shift");
        System.out.println("     • Document expected behavior");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Always validate shift distances");
        System.out.println("  2. Mask bytes before shifting: (b & 0xFF)");
        System.out.println("  3. Use long literals (1L) for 64-bit shifts");
        System.out.println("  4. Understand >> vs >>> differences");
        System.out.println("  5. Check for overflow in multiplication");
        System.out.println("  6. Use bit fields with masks");
        System.out.println("  7. Consider endianness when packing/unpacking");
        System.out.println("  8. Test edge cases (shift by 0, 31, 32, 63, 64)");
        System.out.println("  9. Document shift behavior in APIs");
        System.out.println("  10. Use utility methods from Integer/Long classes");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use Integer.rotateLeft/Right for rotation");
        System.out.println("  • Use Integer.toUnsignedString for unsigned output");
        System.out.println("  • Always mask when converting byte to int");
        System.out.println("  • Validate shift distances before use");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE - byte extraction");
        System.out.println("  byte b = bytes[i];");
        System.out.println("  int value = b << 24; // Sign extends!");
        System.out.println("");
        System.out.println("  // SECURE - byte extraction");
        System.out.println("  byte b = bytes[i];");
        System.out.println("  int value = (b & 0xFF) << 24;");
        System.out.println("");
        System.out.println("  // INSECURE - mask generation");
        System.out.println("  long mask = (1 << length) - 1; // Fails for length=32");
        System.out.println("");
        System.out.println("  // SECURE - mask generation");
        System.out.println("  long mask = (length == 64) ? -1L : ((1L << length) - 1);");

        System.out.println("\n📊 SHIFT BEHAVIOR:");
        System.out.println("  Operation          | Result for 0x80000001 >> 1");
        System.out.println("  -------------------|---------------------------");
        System.out.println("  >> 1 (arithmetic)  | 0xC0000000 (sign extends)");
        System.out.println("  >>> 1 (logical)    | 0x40000000 (zero extends)");
        System.out.println("  << 1               | 0x00000002");

        System.out.println("\n⚠️ REMEMBER: Java only uses the lower 5 bits of shift distance for ints!");
        System.out.println("  For int: shift distance & 0x1F (only bits 0-4)");
        System.out.println("  For long: shift distance & 0x3F (only bits 0-5)");
    }
}