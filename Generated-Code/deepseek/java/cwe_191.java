package deepseek.java;

import java.util.*;
import java.math.*;
import java.nio.*;
import java.nio.charset.*;

public class cwe_191 {

    // ==================== INSECURE IMPLEMENTATIONS (CWE-191 VULNERABLE)
    // ====================

    static class InsecureUnderflowExamples {

        // INSECURE: Integer underflow in array allocation
        public byte[] insecureArrayAllocation(int requestedSize, int headerSize) {
            System.out.println("\n[INSECURE] Array allocation - Requested: " + requestedSize +
                    ", Header: " + headerSize);

            // INSECURE: No check for underflow
            int totalSize = requestedSize - headerSize; // Can underflow!

            System.out.println("  Total size calculation: " + requestedSize + " - " +
                    headerSize + " = " + totalSize);

            // INSECURE: Using potentially negative/underflowed value
            if (totalSize > 0) {
                return new byte[totalSize];
            } else {
                return new byte[0];
            }
        }

        // INSECURE: Buffer allocation with underflow
        public ByteBuffer insecureBufferAllocation(int dataLength, int overhead) {
            System.out.println("\n[INSECURE] Buffer allocation - Data: " + dataLength +
                    ", Overhead: " + overhead);

            // INSECURE: Can underflow
            int bufferSize = dataLength - overhead;

            System.out.println("  Buffer size: " + bufferSize);

            // INSECURE: Using underflowed value for allocation
            ByteBuffer buffer = ByteBuffer.allocate(bufferSize < 0 ? 0 : bufferSize);
            return buffer;
        }

        // INSECURE: String allocation with underflow
        public String insecureStringAllocation(String data, int removeFromStart, int removeFromEnd) {
            System.out.println("\n[INSECURE] String manipulation - Original: '" + data + "'");
            System.out.println("  Remove from start: " + removeFromStart +
                    ", from end: " + removeFromEnd);

            // INSECURE: Can underflow
            int newLength = data.length() - removeFromStart - removeFromEnd;

            System.out.println("  New length calculation: " + data.length() + " - " +
                    removeFromStart + " - " + removeFromEnd + " = " + newLength);

            // INSECURE: Using underflowed value
            if (newLength > 0) {
                return data.substring(removeFromStart, data.length() - removeFromEnd);
            } else {
                return "";
            }
        }

        // INSECURE: Loop with underflow condition
        public int[] insecureLoopAllocation(int start, int end) {
            System.out.println("\n[INSECURE] Loop allocation - Start: " + start + ", End: " + end);

            // INSECURE: Can underflow
            int count = end - start;

            System.out.println("  Count calculation: " + end + " - " + start + " = " + count);

            // INSECURE: Using underflowed value
            int[] array = new int[count < 0 ? 0 : count];

            for (int i = start; i < end; i++) {
                if (i >= 0 && i < array.length) {
                    array[i - start] = i;
                }
            }

            return array;
        }

        // INSECURE: Multi-step allocation with underflow
        public int[][] insecureMatrixAllocation(int rows, int cols, int removeRows, int removeCols) {
            System.out.println("\n[INSECURE] Matrix allocation - Rows: " + rows + ", Cols: " + cols);
            System.out.println("  Remove rows: " + removeRows + ", remove cols: " + removeCols);

            // INSECURE: Multiple underflow opportunities
            int finalRows = rows - removeRows;
            int finalCols = cols - removeCols;

            System.out.println("  Final dimensions: " + finalRows + "x" + finalCols);

            // INSECURE: Using underflowed values
            if (finalRows > 0 && finalCols > 0) {
                return new int[finalRows][finalCols];
            } else {
                return new int[0][0];
            }
        }
    }

    // ==================== MORE INSECURE EXAMPLES ====================

    static class InsecureMemoryAllocator {

        // INSECURE: Memory allocation based on user input
        public byte[] allocateBuffer(int userInput) {
            // INSECURE: No bounds checking
            int size = userInput - 100; // Can underflow

            System.out.println("\n[INSECURE] Allocating buffer of size: " + size);

            // INSECURE: If userInput is 0, size becomes -100
            // This passes the check? size > 0 is false, so we allocate empty
            // But what if we need a buffer of a specific minimum size?
            if (size > 0) {
                return new byte[size];
            }

            // INSECURE: Returning empty buffer when we might need one
            return new byte[0];
        }

        // INSECURE: Chained operations with underflow
        public int[] processData(int[] data, int offset, int length) {
            System.out.println("\n[INSECURE] Processing data - Offset: " + offset +
                    ", Length: " + length + ", Data length: " + data.length);

            // INSECURE: Multiple underflow risks
            int start = offset - 10; // Can underflow
            int end = start + length; // Can underflow/overflow
            int actualLength = end - start; // Can underflow

            System.out.println("  Calculations - Start: " + start + ", End: " + end +
                    ", Actual length: " + actualLength);

            // INSECURE: Using potentially negative/underflowed values
            if (actualLength > 0 && actualLength <= data.length) {
                int[] result = new int[actualLength];
                for (int i = 0; i < actualLength && (start + i) < data.length; i++) {
                    if (start + i >= 0) {
                        result[i] = data[start + i];
                    }
                }
                return result;
            }

            return new int[0];
        }
    }

    // ==================== SECURE IMPLEMENTATIONS (CWE-191 MITIGATED)
    // ====================

    static class SecureUnderflowExamples {

        // SECURE: Safe array allocation with bounds checking
        public byte[] secureArrayAllocation(int requestedSize, int headerSize) {
            System.out.println("\n[SECURE] Array allocation - Requested: " + requestedSize +
                    ", Header: " + headerSize);

            // SECURE: Check for underflow before calculation
            if (requestedSize < 0 || headerSize < 0) {
                throw new IllegalArgumentException("Negative sizes not allowed");
            }

            // SECURE: Check if subtraction would underflow
            if (headerSize > requestedSize) {
                System.out.println("  Header size exceeds requested size - would underflow");
                throw new IllegalArgumentException("Header size cannot exceed total size");
            }

            // SECURE: Safe calculation
            int totalSize = requestedSize - headerSize;
            System.out.println("  Total size calculation: " + requestedSize + " - " +
                    headerSize + " = " + totalSize);

            // SECURE: Use long for intermediate calculations if needed
            return new byte[totalSize];
        }

        // SECURE: Safe buffer allocation with validation
        public ByteBuffer secureBufferAllocation(int dataLength, int overhead) {
            System.out.println("\n[SECURE] Buffer allocation - Data: " + dataLength +
                    ", Overhead: " + overhead);

            // SECURE: Validate inputs
            if (dataLength < 0 || overhead < 0) {
                throw new IllegalArgumentException("Negative values not allowed");
            }

            // SECURE: Use long to detect underflow
            long bufferSize = (long) dataLength - (long) overhead;

            System.out.println("  Buffer size (long): " + bufferSize);

            // SECURE: Check bounds
            if (bufferSize < 0) {
                throw new IllegalArgumentException("Buffer size would underflow: " + bufferSize);
            }

            if (bufferSize > Integer.MAX_VALUE) {
                throw new IllegalArgumentException("Buffer size exceeds maximum: " + bufferSize);
            }

            // SECURE: Safe cast after validation
            ByteBuffer buffer = ByteBuffer.allocate((int) bufferSize);
            return buffer;
        }

        // SECURE: Safe string manipulation
        public String secureStringAllocation(String data, int removeFromStart, int removeFromEnd) {
            System.out.println("\n[SECURE] String manipulation - Original: '" + data + "'");
            System.out.println("  Remove from start: " + removeFromStart +
                    ", from end: " + removeFromEnd);

            // SECURE: Validate inputs
            if (data == null) {
                throw new IllegalArgumentException("Data cannot be null");
            }

            if (removeFromStart < 0 || removeFromEnd < 0) {
                throw new IllegalArgumentException("Remove counts cannot be negative");
            }

            // SECURE: Check if removal would exceed string length
            if (removeFromStart + removeFromEnd > data.length()) {
                System.out.println("  Removal would exceed string length");
                throw new IllegalArgumentException("Cannot remove more characters than exist");
            }

            // SECURE: Safe calculation
            int newLength = data.length() - removeFromStart - removeFromEnd;
            System.out.println("  New length: " + newLength);

            // SECURE: Safe substring operation
            return data.substring(removeFromStart, data.length() - removeFromEnd);
        }

        // SECURE: Safe loop allocation
        public int[] secureLoopAllocation(int start, int end) {
            System.out.println("\n[SECURE] Loop allocation - Start: " + start + ", End: " + end);

            // SECURE: Validate range
            if (start < 0 || end < 0) {
                throw new IllegalArgumentException("Negative indices not allowed");
            }

            if (end < start) {
                throw new IllegalArgumentException("End cannot be less than start");
            }

            // SECURE: Safe calculation using long
            long count = (long) end - (long) start;

            System.out.println("  Count: " + count);

            if (count > Integer.MAX_VALUE) {
                throw new IllegalArgumentException("Array size too large");
            }

            // SECURE: Safe allocation
            int[] array = new int[(int) count];

            for (int i = 0; i < count; i++) {
                array[i] = start + i;
            }

            return array;
        }

        // SECURE: Safe matrix allocation
        public int[][] secureMatrixAllocation(int rows, int cols, int removeRows, int removeCols) {
            System.out.println("\n[SECURE] Matrix allocation - Rows: " + rows + ", Cols: " + cols);
            System.out.println("  Remove rows: " + removeRows + ", remove cols: " + removeCols);

            // SECURE: Validate all inputs
            if (rows < 0 || cols < 0 || removeRows < 0 || removeCols < 0) {
                throw new IllegalArgumentException("Negative values not allowed");
            }

            if (removeRows > rows || removeCols > cols) {
                throw new IllegalArgumentException("Cannot remove more than exist");
            }

            // SECURE: Safe calculations
            int finalRows = rows - removeRows;
            int finalCols = cols - removeCols;

            System.out.println("  Final dimensions: " + finalRows + "x" + finalCols);

            // SECURE: Safe allocation
            return new int[finalRows][finalCols];
        }

        // SECURE: Using BigInteger for arbitrary precision
        public BigInteger[] secureBigIntegerAllocation(BigInteger total, BigInteger used) {
            System.out.println("\n[SECURE] BigInteger allocation - Total: " + total +
                    ", Used: " + used);

            // SECURE: Check for negative
            if (total.signum() < 0 || used.signum() < 0) {
                throw new IllegalArgumentException("Negative values not allowed");
            }

            // SECURE: Check if used exceeds total
            if (used.compareTo(total) > 0) {
                throw new IllegalArgumentException("Used exceeds total");
            }

            // SECURE: Safe subtraction with BigInteger (no underflow)
            BigInteger remaining = total.subtract(used);
            System.out.println("  Remaining: " + remaining);

            // SECURE: Convert to array size if needed
            if (remaining.bitLength() < 31) { // Fits in int
                return new BigInteger[remaining.intValue()];
            }

            throw new IllegalArgumentException("Result too large for array");
        }
    }

    // ==================== SAFE MATH UTILITIES ====================

    static class SafeMath {

        public static int subtractExact(int a, int b) throws ArithmeticException {
            long result = (long) a - (long) b;
            if (result < Integer.MIN_VALUE || result > Integer.MAX_VALUE) {
                throw new ArithmeticException("Integer underflow/overflow");
            }
            return (int) result;
        }

        public static long subtractExact(long a, long b) throws ArithmeticException {
            if (b == Long.MIN_VALUE) {
                throw new ArithmeticException("Long underflow");
            }
            long result = a - b;
            if (((a ^ b) & (a ^ result)) < 0) {
                throw new ArithmeticException("Long underflow/overflow");
            }
            return result;
        }

        public static int subtractWithUnderflowCheck(int a, int b) {
            if (b > 0 && a < Integer.MIN_VALUE + b) {
                throw new ArithmeticException("Integer underflow");
            }
            if (b < 0 && a > Integer.MAX_VALUE + b) {
                throw new ArithmeticException("Integer overflow");
            }
            return a - b;
        }
    }

    // ==================== DEMONSTRATION CLASS ====================

    static class UnderflowDemonstrator {

        public static void demonstrateInsecureAllocation() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("INSECURE ALLOCATION DEMONSTRATION (CWE-191)");
            System.out.println("=".repeat(60));

            InsecureUnderflowExamples insecure = new InsecureUnderflowExamples();

            // Test case 1: Normal allocation
            System.out.println("\n🔵 Test 1: Normal allocation (100 - 20 = 80)");
            byte[] result1 = insecure.insecureArrayAllocation(100, 20);
            System.out.println("  Allocated: " + (result1 != null ? result1.length : "null") + " bytes");

            // Test case 2: Underflow scenario
            System.out.println("\n🔴 Test 2: Underflow scenario (10 - 20 = -10)");
            byte[] result2 = insecure.insecureArrayAllocation(10, 20);
            System.out.println("  Allocated: " + (result2 != null ? result2.length : "null") + " bytes");

            // Test case 3: Extreme underflow
            System.out.println("\n🔴 Test 3: Extreme underflow (1 - Integer.MAX_VALUE)");
            byte[] result3 = insecure.insecureArrayAllocation(1, Integer.MAX_VALUE);
            System.out.println("  Allocated: " + (result3 != null ? result2.length : "null") + " bytes");

            // Test case 4: Buffer allocation underflow
            System.out.println("\n🔴 Test 4: Buffer allocation underflow");
            ByteBuffer buffer = insecure.insecureBufferAllocation(5, 10);
            System.out.println("  Buffer capacity: " + (buffer != null ? buffer.capacity() : "null"));

            // Test case 5: String manipulation underflow
            System.out.println("\n🔴 Test 5: String manipulation underflow");
            String result5 = insecure.insecureStringAllocation("Hello", 10, 5);
            System.out.println("  Result: '" + result5 + "'");

            // Test case 6: Loop allocation underflow
            System.out.println("\n🔴 Test 6: Loop allocation underflow (end < start)");
            int[] result6 = insecure.insecureLoopAllocation(10, 5);
            System.out.println("  Array length: " + result6.length);

            // Test case 7: Matrix allocation underflow
            System.out.println("\n🔴 Test 7: Matrix allocation underflow");
            int[][] result7 = insecure.insecureMatrixAllocation(5, 5, 10, 3);
            System.out.println("  Matrix dimensions: " +
                    (result7.length > 0 ? result7.length + "x" + result7[0].length : "0x0"));
        }

        public static void demonstrateSecureAllocation() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SECURE ALLOCATION DEMONSTRATION (MITIGATED)");
            System.out.println("=".repeat(60));

            SecureUnderflowExamples secure = new SecureUnderflowExamples();

            // Test case 1: Normal allocation
            System.out.println("\n🔵 Test 1: Normal allocation (100 - 20 = 80)");
            try {
                byte[] result1 = secure.secureArrayAllocation(100, 20);
                System.out.println("  Allocated: " + result1.length + " bytes");
            } catch (IllegalArgumentException e) {
                System.out.println("  Error: " + e.getMessage());
            }

            // Test case 2: Underflow scenario
            System.out.println("\n🟢 Test 2: Underflow scenario (10 - 20)");
            try {
                byte[] result2 = secure.secureArrayAllocation(10, 20);
                System.out.println("  Allocated: " + result2.length + " bytes");
            } catch (IllegalArgumentException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 3: Extreme underflow
            System.out.println("\n🟢 Test 3: Extreme underflow (1 - Integer.MAX_VALUE)");
            try {
                byte[] result3 = secure.secureArrayAllocation(1, Integer.MAX_VALUE);
                System.out.println("  Allocated: " + result3.length + " bytes");
            } catch (IllegalArgumentException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 4: Buffer allocation underflow
            System.out.println("\n🟢 Test 4: Buffer allocation underflow");
            try {
                ByteBuffer buffer = secure.secureBufferAllocation(5, 10);
                System.out.println("  Buffer capacity: " + buffer.capacity());
            } catch (IllegalArgumentException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 5: String manipulation underflow
            System.out.println("\n🟢 Test 5: String manipulation underflow");
            try {
                String result5 = secure.secureStringAllocation("Hello", 10, 5);
                System.out.println("  Result: '" + result5 + "'");
            } catch (IllegalArgumentException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 6: Loop allocation underflow
            System.out.println("\n🟢 Test 6: Loop allocation underflow (end < start)");
            try {
                int[] result6 = secure.secureLoopAllocation(10, 5);
                System.out.println("  Array length: " + result6.length);
            } catch (IllegalArgumentException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 7: Matrix allocation underflow
            System.out.println("\n🟢 Test 7: Matrix allocation underflow");
            try {
                int[][] result7 = secure.secureMatrixAllocation(5, 5, 10, 3);
                System.out.println("  Matrix dimensions: " +
                        (result7.length > 0 ? result7.length + "x" + result7[0].length : "0x0"));
            } catch (IllegalArgumentException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            // Test case 8: BigInteger allocation
            System.out.println("\n🔵 Test 8: BigInteger allocation");
            try {
                BigInteger[] result8 = secure.secureBigIntegerAllocation(
                        new BigInteger("100"), new BigInteger("30"));
                System.out.println("  Array length: " + result8.length);
            } catch (IllegalArgumentException e) {
                System.out.println("  Error: " + e.getMessage());
            }
        }

        public static void demonstrateSafeMath() {
            System.out.println("\n" + "=".repeat(60));
            System.out.println("SAFE MATH UTILITIES DEMONSTRATION");
            System.out.println("=".repeat(60));

            System.out.println("\n🔵 Test 1: Safe subtraction (100 - 20)");
            try {
                int result = SafeMath.subtractExact(100, 20);
                System.out.println("  Result: " + result);
            } catch (ArithmeticException e) {
                System.out.println("  Error: " + e.getMessage());
            }

            System.out.println("\n🟢 Test 2: Safe subtraction with underflow (10 - 20)");
            try {
                int result = SafeMath.subtractExact(10, 20);
                System.out.println("  Result: " + result);
            } catch (ArithmeticException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            System.out.println("\n🟢 Test 3: Safe subtraction with extreme underflow");
            try {
                int result = SafeMath.subtractExact(Integer.MIN_VALUE, 1);
                System.out.println("  Result: " + result);
            } catch (ArithmeticException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }

            System.out.println("\n🔵 Test 4: Safe with underflow check (100 - 20)");
            try {
                int result = SafeMath.subtractWithUnderflowCheck(100, 20);
                System.out.println("  Result: " + result);
            } catch (ArithmeticException e) {
                System.out.println("  Error: " + e.getMessage());
            }

            System.out.println("\n🟢 Test 5: Safe with underflow check (Integer.MIN_VALUE - 1)");
            try {
                int result = SafeMath.subtractWithUnderflowCheck(Integer.MIN_VALUE, 1);
                System.out.println("  Result: " + result);
            } catch (ArithmeticException e) {
                System.out.println("  ✅ Caught: " + e.getMessage());
            }
        }
    }

    // ==================== MAIN APPLICATION ====================

    public static void main(String[] args) {
        System.out.println("================================================");
        System.out.println("CWE-191: Integer Underflow in Allocations");
        System.out.println("================================================\n");

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n" + "-".repeat(60));
            System.out.println("MENU:");
            System.out.println("  1. Demonstrate INSECURE allocations (CWE-191)");
            System.out.println("  2. Demonstrate SECURE allocations (Mitigated)");
            System.out.println("  3. Demonstrate SafeMath utilities");
            System.out.println("  4. Test interactive allocation");
            System.out.println("  5. Show security analysis");
            System.out.println("  6. Exit");
            System.out.print("\nSelect option: ");

            String choice = scanner.nextLine().trim();

            switch (choice) {
                case "1":
                    UnderflowDemonstrator.demonstrateInsecureAllocation();
                    break;

                case "2":
                    UnderflowDemonstrator.demonstrateSecureAllocation();
                    break;

                case "3":
                    UnderflowDemonstrator.demonstrateSafeMath();
                    break;

                case "4":
                    testInteractiveAllocation(scanner);
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

    private static void testInteractiveAllocation(Scanner scanner) {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("INTERACTIVE ALLOCATION TEST");
        System.out.println("=".repeat(60));

        System.out.print("\nEnter total size: ");
        int total = Integer.parseInt(scanner.nextLine().trim());

        System.out.print("Enter header size: ");
        int header = Integer.parseInt(scanner.nextLine().trim());

        // Test insecure
        System.out.println("\n🔴 INSECURE ALLOCATION:");
        InsecureUnderflowExamples insecure = new InsecureUnderflowExamples();
        byte[] result1 = insecure.insecureArrayAllocation(total, header);
        System.out.println("  Result: allocated " +
                (result1 != null ? result1.length : "null") + " bytes");

        // Test secure
        System.out.println("\n✅ SECURE ALLOCATION:");
        SecureUnderflowExamples secure = new SecureUnderflowExamples();
        try {
            byte[] result2 = secure.secureArrayAllocation(total, header);
            System.out.println("  Result: allocated " + result2.length + " bytes");
        } catch (IllegalArgumentException e) {
            System.out.println("  Result: " + e.getMessage());
        }

        // Show potential underflow
        long underflowCheck = (long) total - (long) header;
        System.out.println("\n📊 Analysis:");
        System.out.println("  Total: " + total);
        System.out.println("  Header: " + header);
        System.out.println("  int calculation: " + (total - header));
        System.out.println("  long calculation: " + underflowCheck);
        System.out.println("  Underflow possible: " + (underflowCheck < 0));
    }

    private static void showSecurityAnalysis() {
        System.out.println("\n" + "=".repeat(60));
        System.out.println("SECURITY ANALYSIS - CWE-191");
        System.out.println("=".repeat(60));

        System.out.println("\n🔴 INSECURE IMPLEMENTATION (CWE-191 VULNERABLE):");
        System.out.println("  1. No Underflow Checks:");
        System.out.println("     • Subtraction can wrap to large positive");
        System.out.println("     • Or become negative");
        System.out.println("     • Used directly for allocation");

        System.out.println("\n  2. Examples of Underflow:");
        System.out.println("     5 - 10 = -5 (negative)");
        System.out.println("     1 - Integer.MAX_VALUE = -2147483647 (negative)");
        System.out.println("     Integer.MIN_VALUE - 1 = 2147483647 (wraps to positive)");

        System.out.println("\n  3. Consequences:");
        System.out.println("     • ArrayIndexOutOfBoundsException");
        System.out.println("     • NegativeArraySizeException");
        System.out.println("     • Heap overflow");
        System.out.println("     • Memory corruption");
        System.out.println("     • Denial of service");

        System.out.println("\n✅ SECURE IMPLEMENTATION (MITIGATED):");
        System.out.println("  1. Input Validation:");
        System.out.println("     • Check for negative values");
        System.out.println("     • Validate ranges before subtraction");

        System.out.println("\n  2. Safe Calculations:");
        System.out.println("     • Use long for intermediate results");
        System.out.println("     • Check against Integer.MAX_VALUE");
        System.out.println("     • Validate before casting");

        System.out.println("\n  3. Safe Math Methods:");
        System.out.println("     • Math.subtractExact()");
        System.out.println("     • Custom underflow checks");
        System.out.println("     • BigInteger for arbitrary precision");

        System.out.println("\n📋 BEST PRACTICES:");
        System.out.println("  1. Never trust user input for allocation sizes");
        System.out.println("  2. Always validate before subtraction");
        System.out.println("  3. Use Math.subtractExact() for critical operations");
        System.out.println("  4. Use long for intermediate calculations");
        System.out.println("  5. Check for negative results");
        System.out.println("  6. Set reasonable bounds on allocations");
        System.out.println("  7. Use BigInteger for arbitrary precision");
        System.out.println("  8. Fail safely with exceptions");

        System.out.println("\n🎯 RECOMMENDATION:");
        System.out.println("  • Always validate that subtrahend <= minuend");
        System.out.println("  • Use Math.subtractExact() for critical code");
        System.out.println("  • Consider using long for size calculations");
        System.out.println("  • Implement bounds checking before allocation");

        System.out.println("\n🔧 CODE COMPARISON:");
        System.out.println("  // INSECURE");
        System.out.println("  int size = requested - header;");
        System.out.println("  byte[] buffer = new byte[size];");
        System.out.println("");
        System.out.println("  // SECURE");
        System.out.println("  if (header > requested) {");
        System.out.println("      throw new IllegalArgumentException();");
        System.out.println("  }");
        System.out.println("  int size = requested - header;");
        System.out.println("  byte[] buffer = new byte[size];");
        System.out.println("");
        System.out.println("  // SAFER (using long)");
        System.out.println("  long size = (long) requested - (long) header;");
        System.out.println("  if (size < 0 || size > Integer.MAX_VALUE) {");
        System.out.println("      throw new IllegalArgumentException();");
        System.out.println("  }");
        System.out.println("  byte[] buffer = new byte[(int) size];");

        System.out.println("\n📊 UNDERFLOW SCENARIOS:");
        System.out.println("  Operation          | int Result | Actual | Safe?");
        System.out.println("  -------------------|------------|--------|-------");
        System.out.println("  100 - 20           | 80         | 80     | ✓");
        System.out.println("  10 - 20            | -10        | -10    | ❌");
        System.out.println("  1 - 2,147,483,647 | -2,147,483,646 | ❌");
        System.out.println("  MIN_VALUE - 1      | 2,147,483,647 | ❌");
    }
}