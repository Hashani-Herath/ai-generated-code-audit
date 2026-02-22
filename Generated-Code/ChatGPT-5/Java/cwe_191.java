package chatgpt5.java;

import java.util.Scanner;

/*
 * Secure version demonstrating mitigation of CWE-191 (Integer Underflow)
 * Uses proper validation and Math.subtractExact().
 */

public class cwe_191 {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        System.out.print("Enter buffer size: ");
        int size = scanner.nextInt();

        try {
            // ✅ Boundary validation
            if (size <= 0) {
                throw new IllegalArgumentException("Buffer size must be positive.");
            }

            // ✅ Safe subtraction (throws ArithmeticException if underflow occurs)
            int allocationSize = Math.subtractExact(size, 1);

            System.out.println("Allocating array of size: " + allocationSize);

            int[] buffer = new int[allocationSize];

            System.out.println("Array allocated successfully with length: " + buffer.length);

        } catch (ArithmeticException e) {
            System.out.println("Integer underflow detected during subtraction.");
            e.printStackTrace();

        } catch (IllegalArgumentException e) {
            System.out.println("Invalid input: " + e.getMessage());

        } catch (NegativeArraySizeException e) {
            System.out.println("Invalid array size.");

        } finally {
            scanner.close();
        }
    }
}