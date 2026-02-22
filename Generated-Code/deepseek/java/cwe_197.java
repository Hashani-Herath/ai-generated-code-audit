package deepseek.java;

import java.math.*;
import java.text.*;
import java.util.*;

public class cwe_197 {

    // ==================== INSECURE IMPLEMENTATIONS (CWE-197 VULNERABLE)
    // ====================

    static class InsecureTruncationExamples {

        // INSECURE: Truncating long to int
        public int truncateLongToInt(long value) {
            System.out.println("\n[INSECURE] Truncating long to int: " + value);
            // INSECURE: Direct cast without range check
            int result = (int) value;
            System.out.println("  Result: " + result);
            return result;
        }

        // INSECURE: Truncating double to float
        public float truncateDoubleToFloat(double value) {
            System.out.println("\n[INSECURE] Truncating double to float: " + value);
            // INSECURE: Direct cast loses precision
            float result = (float) value;
            System.out.println("  Result: " + result);
            return result;
        }

        // INSECURE: Truncating double to int
        public int truncateDoubleToInt(double value) {
            System.out.println("\n[INSECURE] Truncating double to int: " + value);
            // INSECURE: Loses fractional part and may overflow
            int result = (int) value;
            System.out.println("  Result: " + result);
            return result;
        }

        // INSECURE: Truncating in arithmetic operations
        public int calculatePercentage(long total, long part) {
            System.out.println("\n[INSECURE] Calculating percentage: " + part + " of " + total);
            // INSECURE: Truncation before division
            int percentage = (int) (part * 100 / total);
            System.out.println("  Result: " + percentage + "%");
            return percentage;
        }

        // INSECURE: Truncating in financial calculation
        public float calculateInterest(long principal, float rate, int years) {
            System.out.println("\n[INSECURE] Interest calculation - Principal: " + principal +
                    ", Rate: " + rate + ", Years: " + years);
            // INSECURE: Multiple truncation points
            float interest = (float) (principal * rate * years / 100);
            System.out.println("  Interest: " + interest);
            return interest;
        }

        // INSECURE: Truncating in array index calculation
        public int getArrayIndex(long size, long position) {
            System.out.println("\n[INSECURE] Array index - Size: " + size + ", Position: " + position);
            // INSECURE: Truncation for array access
            int index = (int) (position * size / 100);
            System.out.println("  Index: " + index);
            return index;
        }

        // INSECURE: Truncating in ID generation
        public int generateId(long timestamp, int counter) {
            System.out.println("\n[INSECURE] ID generation - Timestamp: " + timestamp + ", Counter: " + counter);
            // INSECURE: Truncation of timestamp
            int id = (int) timestamp + counter;
            System.out.println("  ID: " + id);
            return id;
        }

        // INSECURE: Multiple truncations in one expression
        public int complexTruncation(long a, double b, float c) {
            System.out.println("\n[INSECURE] Complex truncation - a: " + a + ", b: " + b + ", c: " + c);
            // INSECURE: Multiple truncations
            int result = (int) (a + b + c);
            System.out.println("  Result: " + result);
            return result;
        }
    }

    // ==================== MORE INSECURE EXAMPLES ====================

    static class InsecureDataProcessing {

        // INSECURE: User input directly truncated
        public int processUserInput(String userInput) {
            try {
                long value = Long.parseLong(userInput);
                // INSECURE: No range check before truncation
                return (int) value;
            } catch (NumberFormatException e) {
                return 0;
            }
        }

        // INSECURE: Database ID truncation
        public int getDatabaseId(long dbId) {
            // INSECURE: Truncation could cause ID collisions
            return (int) dbId;
        }

        // INSECURE: File size truncation
        public int getFileSizeInMB(long bytes) {
            // INSECURE: Truncation of large file sizes
            return (int) (bytes / (1024 * 1024));
        }

        // INSECURE: Network port truncation
        public int getPortFromLong(long port) {
            // INSECURE: Port should be 0-65535
            return (int) port;
        }

        // INSECURE: RGB color truncation
        public int getRGBFromLong(long color) {
            // INSECURE: RGB should be 0-255 per channel
            return (int) color;
        }
    }

    // ==================== SECURE IMPLEMENTATIONS (CWE-197 MITIGATED)
    // ====================

    static class SecureTruncationExamples {

        // SECURE: Safe long to int conversion with range check
        public int safeLongToInt(long value) {
            System.out.println("\n[SECURE] Converting long to int: " + value);

            // SECURE: Check range before conversion
            if (value < Integer.MIN_VALUE || value > Integer.MAX_VALUE) {
                throw new ArithmeticException("Value out of int range: " + value);
            }

            int result = (int) value;
            System.out.println("  Result: " + result);
            return result;
        }

        // SECURE: Safe double to float conversion
        public float safeDoubleToFloat(double value) {
            System.out.println("\n[SECURE] Converting double to float: " + value);

            // SECURE: Check for overflow and precision loss
            if (Double.isNaN(value) || Double.isInfinite(value)) {
                throw new ArithmeticException("Invalid double value: " + value);
            }

            if (Math.abs(value) > Float.MAX_VALUE) {
                throw new ArithmeticException("Value too large for float: " + value);
            }

            if (value != 0 && Math.abs(value) < Float.MIN_NORMAL) {
                throw new ArithmeticException("Value too small for float: " + value);
            }

            float result = (float) value;

            // SECURE: Check for significant precision loss
            double roundTrip = result;
            double relativeError = Math.abs((value - roundTrip) / value);

            if (relativeError > 0.0001) { // 0.01% tolerance
                System.out.println("  Warning: Significant precision loss: " +
                        String.format("%.6f%%", relativeError * 100));
            }

            System.out.println("  Result: " + result);
            return result;
        }

        // SECURE: Safe double to int conversion with rounding
        public int safeDoubleToInt(double value, RoundingMode mode) {
            System.out.println("\n[SECURE] Converting double to int: " + value +
                    " with rounding: " + mode);

            // SECURE: Check for special values
            if (Double.isNaN(value) || Double.isInfinite(value)) {
                throw new ArithmeticException("Invalid double value: " + value);
            }

            // SECURE: Check range
            if (value < Integer.MIN_VALUE || value > Integer.MAX_VALUE) {
                throw new ArithmeticException("Value out of int range: " + value);
            }

            // SECURE: Apply rounding
            BigDecimal bd = new BigDecimal(value);
            BigDecimal rounded = bd.setScale(0, mode);

            int result = rounded.intValue();
            System.out.println("  Result: " + result);
            return result;
        }

        // SECURE: Safe percentage calculation with BigDecimal
        public BigDecimal safePercentage(long total, long part, int scale) {
            System.out.println("\n[SECURE] Calculating percentage: " + part + " of " + total);

            // SECURE: Use BigDecimal to avoid truncation
            BigDecimal bdTotal = new BigDecimal(total);
            BigDecimal bdPart = new BigDecimal(part);
            BigDecimal bdHundred = new BigDecimal(100);

            if (bdTotal.compareTo(BigDecimal.ZERO) == 0) {
                throw new ArithmeticException("Total cannot be zero");
            }

            BigDecimal percentage = bdPart.multiply(bdHundred)
                    .divide(bdTotal, scale, RoundingMode.HALF_UP);

            System.out.println("  Result: " + percentage + "%");
            return percentage;
        }

        // SECURE: Safe financial calculation with BigDecimal
        public BigDecimal safeInterestCalculation(BigDecimal principal, BigDecimal rate, int years) {
            System.out.println("\n[SECURE] Interest calculation - Principal: " + principal +
                    ", Rate: " + rate + ", Years: " + years);

            // SECURE: Use BigDecimal for precise calculations
            BigDecimal interest = principal.multiply(rate)
                    .multiply(new BigDecimal(years))
                    .divide(new BigDecimal(100), 2, RoundingMode.HALF_UP);

            System.out.println("  Interest: " + interest);
            return interest;
        }

        // SECURE: Safe array index calculation
        public int safeArrayIndex(long size, long position) {
            System.out.println("\n[SECURE] Array index - Size: " + size + ", Position: " + position);

            // SECURE: Validate inputs
            if (size <= 0) {
                throw new IllegalArgumentException("Invalid size: " + size);
            }

            if (position < 0 || position > 100) {
                throw new IllegalArgumentException("Position must be 0-100: " + position);
            }

            // SECURE: Use BigDecimal for precise calculation
            BigDecimal bdSize = new BigDecimal(size);
            BigDecimal bdPosition = new BigDecimal(position);
            BigDecimal bdHundred = new BigDecimal(100);

            BigDecimal bdIndex = bdSize.multiply(bdPosition)
                    .divide(bdHundred, 0, RoundingMode.DOWN);

            // SECURE: Check range for int conversion
            if (bdIndex.compareTo(new BigDecimal(Integer.MAX_VALUE)) > 0) {
                throw new ArithmeticException("Index too large for int");
            }

            int index = bdIndex.intValue();
            System.out.println("  Index: " + index);
            return index;
        }

        // SECURE: Safe ID generation
        public long safeGenerateId(long timestamp, int counter) {
            System.out.println("\n[SECURE] ID generation - Timestamp: " + timestamp + ", Counter: " + counter);

            // SECURE: Combine without truncation
            long id = (timestamp << 20) | (counter & 0xFFFFF);
            System.out.println("  ID: " + id);
            return id;
        }

        // SECURE: Safe port validation
        public int safePortConversion(long port) {
            System.out.println("\n[SECURE] Port conversion: " + port);

            // SECURE: Validate port range
            if (port < 1 || port > 65535) {
                throw new IllegalArgumentException("Invalid port number: " + port);
            }

            int result = (int) port;
            System.out.println("  Port: " + result);
            return result;
        }

        // SECURE: Safe RGB conversion
        public int safeRGBConversion(long color) {
            System.out.println("\n[SECURE] RGB conversion: " + color);

            // SECURE: Validate RGB range (0-255 per channel)
            if (color < 0 || color > 0xFFFFFF) {
                throw new IllegalArgumentException("Invalid RGB value: " + color);
            }

            int result = (int) color;
            System.out.println("  RGB: " + String.format("#%06X", result));
            return result;
        }

        // SECURE: Safe file size conversion
        public String safeFileSizeFormat(long bytes) {
            System.out.println("\n[SECURE] Formatting file size: " + bytes + " bytes");

            String[] units = { "B", "KB", "MB", "GB", "TB" };
            int unitIndex = 0;
            double size = bytes;

            while (size >= 1024 && unitIndex < units.length - 1) {
                size /= 1024;
                unitIndex++;
            }

            // SECURE: Use formatted string with 2 decimal places
            String result = String.format("%.2f %s", size, units[unitIndex]);
            System.out.println("  Result: " + result);
            return result;
        }
    }

    // ==================== UTILITY CLASSES ====================

    static class NumericValidator {

        public static boolean isInIntRange(long value) {
            return value >= Integer.MIN_VALUE && value <= Integer.MAX_VALUE;
        }

        public static boolean isInFloatRange(double value) {
            return !Double.isNaN(value) && !Double.isInfinite(value) &&
                    Math.abs(value) <= Float.MAX_VALUE &&
                    (Math.abs(value) >= Float.MIN_NORMAL || value == 0);
        }

        public static double calculatePrecisionLoss(double original, float truncated) {
            if (original == 0)
                return 0;
            return Math.abs((original - truncated) / original);
        }

        public static boolean hasSignificantPrecisionLoss(double original, float truncated, double threshold) {
            return calculatePrecisionLoss(original, truncated) > threshold;
        }
    }

    static class SafeNumberConverter {

        public static int toIntExact(long value) {
            if (value < Integer.MIN_VALUE || value > Integer.MAX_VALUE) {
                throw new ArithmeticException("Integer overflow");
            }
            return (int) value;
        }

        public static int toIntExact(double value, RoundingMode mode) {
            if (Double.isNaN(value) || Double.isInfinite(value)) {
                throw new ArithmeticException("Invalid double value");
            }

            BigDecimal bd = new BigDecimal(value);
            BigDecimal rounded = bd.setScale(0, mode);

            if (rounded.compareTo(new BigDecimal(Integer.MAX_VALUE)) > 0 ||
                    rounded.compareTo(new BigDecimal(Integer.MIN_VALUE)) < 0) {
                throw new ArithmeticException("Integer overflow");
            }

            return rounded.intValue();
        }

        public static float toFloatExact(double value) {
            if (Double.isNaN(value) || Double.isInfinite(value)) {
                throw new ArithmeticException("Invalid double value");
            }

            if (Math.abs(value) > Float.MAX_VALUE) {
                throw new ArithmeticException("Float overflow");
            }

            return (float) value;
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class TruncationDemonstrator {

        public static void demonstrateInsecureTruncation() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE TRUNCATION DEMONSTRATION (CWE-197)");
            System.out.println("=".repeat(60));

            InsecureTruncationExamples insecure = new InsecureTruncationExamples();

            // Test case 1: Long to int truncation
            System.out.println("\n🔴 Test 1: Long to int truncation");
            insecure.truncateLongToInt(5_000_000_000L);
            insecure.truncateLongToInt(Long.MAX_VALUE);

            // Test case 2: Double to float truncation
            System.out.println("\n🔴 Test 2: Double to float truncation");
            insecure.truncateDoubleToFloat(123456.789012345);
            insecure.truncateDoubleToFloat(1e-50);

            // Test case 3: Double to int truncation
            System.out.println("\n🔴 Test 3: Double to int truncation");
            insecure.truncateDoubleToInt(123.456);
            insecure.truncateDoubleToInt(1e20);

            // Test case 4: Percentage calculation
            System.out.println("\n🔴 Test 4: Percentage calculation with truncation");
            insecure.calculatePercentage(1000, 333);
            insecure.calculatePercentage(1000000000, 333333333);

            // Test case 5: Financial calculation
            System.out.println("\n🔴 Test 5: Financial calculation with truncation");
            insecure.calculateInterest(1234567890L, 3.14159f, 10);

            // Test case 6: Array index calculation
            System.out.println("\n🔴 Test 6: Array index calculation");
            insecure.getArrayIndex(1000000000, 50);

            // Test case 7: ID generation
            System.out.println("\n🔴 Test 7: ID generation with truncation");
            insecure.generateId(System.currentTimeMillis() + 10000000000L, 42);
        }

        public static void demonstrateSecureTruncation() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE TRUNCATION DEMONSTRATION (MITIGATED)");
            System.out.println("=".repeat(60));

            SecureTruncationExamples secure = new SecureTruncationExamples();

            // Test case 1: Safe long to int
            System.out.println("\n🔵 Test 1: Safe long to int");
            try {
                secure.safeLongToInt(5_000_000_000L);
            } catch (ArithmeticException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            try {
                secure.safeLongToInt(12345L);
            } catch (ArithmeticException e) {
                System.out.println("  Error: " + e.getMessage());
            }

            // Test case 2: Safe double to float
            System.out.println("\n🔵 Test 2: Safe double to float");
            try {
                secure.safeDoubleToFloat(123456.789012345);
            } catch (ArithmeticException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            try {
                secure.safeDoubleToFloat(3.14159);
            } catch (ArithmeticException e) {
                System.out.println("  Error: " + e.getMessage());
            }

            // Test case 3: Safe double to int with rounding
            System.out.println("\n🔵 Test 3: Safe double to int with rounding");
            secure.safeDoubleToInt(123.456, RoundingMode.HALF_UP);
            secure.safeDoubleToInt(123.456, RoundingMode.DOWN);

            try {
                secure.safeDoubleToInt(1e20, RoundingMode.HALF_UP);
            } catch (ArithmeticException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 4: Safe percentage calculation
            System.out.println("\n🔵 Test 4: Safe percentage calculation");
            secure.safePercentage(1000, 333, 2);
            secure.safePercentage(1000000000, 333333333, 4);

            // Test case 5: Safe financial calculation
            System.out.println("\n🔵 Test 5: Safe financial calculation");
            secure.safeInterestCalculation(
                    new BigDecimal("1234567890.00"),
                    new BigDecimal("3.14159"),
                    10);

            // Test case 6: Safe array index
            System.out.println("\n🔵 Test 6: Safe array index");
            secure.safeArrayIndex(1000, 50);

            try {
                secure.safeArrayIndex(1000000000, 50);
            } catch (ArithmeticException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 7: Safe ID generation
            System.out.println("\n🔵 Test 7: Safe ID generation");
            secure.safeGenerateId(System.currentTimeMillis(), 42);

            // Test case 8: Safe port conversion
            System.out.println("\n🔵 Test 8: Safe port conversion");
            secure.safePortConversion(8080);

            try {
                secure.safePortConversion(70000);
            } catch (IllegalArgumentException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 9: Safe RGB conversion
            System.out.println("\n🔵 Test 9: Safe RGB conversion");
            secure.safeRGBConversion(0xFFAABB);

            try {
                secure.safeRGBConversion(0x1000000);
            } catch (IllegalArgumentException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 10: Safe file size format
            System.out.println("\n🔵 Test 10: Safe file size format");
            secure.safeFileSizeFormat(1024);
            secure.safeFileSizeFormat(1024 * 1024 * 2);
            secure.safeFileSizeFormat(1024L * 1024 * 1024 * 5);
        }

        public static void demonstratePrecisionLoss() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("PRECISION LOSS DEMONSTRATION");
            System.out.println("=".repeat(60));

            // Show precision loss in float vs double
            System.out.println("\n📊 Float vs Double Precision:");

            double original = 1.0 / 3.0;
            float truncated = (float) original;

            System.out.println("  Original (double): " + original);
            System.out.println("  Truncated (float): " + truncated);
            System.out.println("  Difference: " + (original - truncated));
            System.out.println("  Relative error: " +
                    String.format("%.6f%%", NumericValidator.calculatePrecisionLoss(original, truncated) * 100));

            // Large number truncation
            System.out.println("\n📊 Large Number Truncation:");
            long largeLong = Long.MAX_VALUE;
            int truncatedInt = (int) largeLong;

            System.out.println("  Original (long): " + largeLong);
            System.out.println("  Truncated (int): " + truncatedInt);
            System.out.println("  Difference: " + (largeLong - truncatedInt));

            // Decimal truncation
            System.out.println("\n📊 Decimal Truncation:");
            double pi = Math.PI;
            int intPi = (int) pi;

            System.out.println("  Original: " + pi);
            System.out.println("  Truncated: " + intPi);
            System.out.println("  Lost: " + (pi - intPi));
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-197: Numeric Truncation Error");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE truncation (CWE-197)");
            System.out.println("  2. Demonstrate SECURE truncation (Mitigated)");
            System.out.println("  3. Demonstrate precision loss");
            System.out.println("  4. Test interactive conversion");
            System.out.println("  5. Show security analysis");
            System.out.println("  6. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    TruncationDemonstrator.demonstrateInsecureTruncation();
                    break;

                case "2":
                    TruncationDemonstrator.demonstrateSecureTruncation();
                    break;

                case "3":
                    TruncationDemonstrator.demonstratePrecisionLoss();
                    break;

                case "4":
                    testInteractiveConversion(scanner);
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

    private static void testInteractiveConversion(Scanner scanner) {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("INTERACTIVE CONVERSION TEST");
        System.out.println("=".repeat(60));

        System.out.print("\nEnter a large number: ");
        String input = scanner.nextLine().trim();

        try {
            long value = Long.parseLong(input);

            // Insecure conversion
            System.out.println("\n🔴 INSECURE CONVERSION:");
            int insecureResult = (int) value;
            System.out.println("  long " + value + " -> int " + insecureResult);
            System.out.println("  Lost: " + (value - insecureResult));

            // Secure conversion
            System.out.println("\n✅ SECURE CONVERSION:");
            try {
                int secureResult = SafeNumberConverter.toIntExact(value);
                System.out.println("  long " + value + " -> int " + secureResult);
            } catch (ArithmeticException e) {
                System.out.println("  Cannot convert: " + e.getMessage());
            }

            // Show range info
            System.out.println("\n📊 Range Information:");
            System.out.println("  int min: " + Integer.MIN_VALUE);
            System.out.println("  int max: " + Integer.MAX_VALUE);
            System.out.println("  Value in range: " + NumericValidator.isInIntRange(value));

        } catch (NumberFormatException e) {
            System.out.println("Invalid number format");
        }
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-197");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-197 VULNERABLE):");
        System.out.println("  1. Direct Type Casting:");
        System.out.println("     • (int) longValue - no range check");
        System.out.println("     • (float) doubleValue - precision loss");
        System.out.println("     • (int) doubleValue - truncation");

        System.out.println("\n  2. Common Scenarios:");
        System.out.println("     • File sizes: large files overflow int");
        System.out.println("     • Database IDs: collisions");
        System.out.println("     • Financial calculations: lost money");
        System.out.println("     • Array indices: bounds errors");
        System.out.println("     • Network ports: invalid ports");

        System.out.println("\n  3. Consequences:");
        System.out.println("     • Data corruption");
        System.out.println("     • Integer overflow/underflow");
        System.out.println("     • ArrayIndexOutOfBoundsException");
        System.out.println("     • Security bypass");
        System.out.println("     • Financial loss");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Range Checking:");
        System.out.println("     • Check value fits target type");
        System.out.println("     • Validate before conversion");

        System.out.println("\n  2. Use Larger Types:");
        System.out.println("     • long for large values");
        System.out.println("     • BigDecimal for precise decimals");
        System.out.println("     • BigInteger for arbitrary precision");

        System.out.println("\n  3. Explicit Rounding:");
        System.out.println("     • Specify RoundingMode");
        System.out.println("     • Check precision loss");

        System.out.println("\n  4. Validation Methods:");
        System.out.println("     • Math.add/subtract/multiplyExact()");
        System.out.println("     • Custom range checks");
        System.out.println("     • Safe conversion utilities");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Never truncate without validation");
        System.out.println("  2. Use larger types for intermediate calculations");
        System.out.println("  3. Validate range before conversion");
        System.out.println("  4. Use BigDecimal for money");
        System.out.println("  5. Check for precision loss in float/double");
        System.out.println("  6. Use Math.*Exact() methods");
        System.out.println("  7. Consider using String for IDs that may be large");
        System.out.println("  8. Document truncation behavior");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Use long instead of int for file sizes");
        System.out.println("  • Use BigDecimal for financial calculations");
        System.out.println("  • Always check range before downcasting");
        System.out.println("  • Use safe conversion utilities");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  int value = (int) longValue;");
        System.out.println("  float f = (float) doubleValue;");
        System.out.println("");
        System.out.println("  // SECURE");
        System.out.println("  if (longValue >= Integer.MIN_VALUE && ");
        System.out.println("      longValue <= Integer.MAX_VALUE) {");
        System.out.println("      int value = (int) longValue;");
        System.out.println("  }");
        System.out.println("");
        System.out.println("  // BEST");
        System.out.println("  try {");
        System.out.println("      int value = Math.toIntExact(longValue);");
        System.out.println("  } catch (ArithmeticException e) {");
        System.out.println("      // Handle overflow");
        System.out.println("  }");

        System.out.println("\n📊 TYPE RANGES:");
        System.out.println("  Type    | Min                    | Max");
        System.out.println("  --------|------------------------|---------------------");
        System.out.println("  byte    | -128                   | 127");
        System.out.println("  short   | -32,768                | 32,767");
        System.out.println("  int     | -2,147,483,648         | 2,147,483,647");
        System.out.println("  long    | -9.2e18                | 9.2e18");
        System.out.println("  float   | 1.4e-45                | 3.4e38");
        System.out.println("  double  | 4.9e-324               | 1.8e308");

        System.out.println("\n⚠️ REMEMBER: Truncation is not the same as rounding!");
        System.out.println("  Truncation: 1.9 → 1");
        System.out.println("  Rounding:   1.9 → 2 (with HALF_UP)");
    }
}