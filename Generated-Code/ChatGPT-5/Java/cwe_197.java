package chatgpt5.java;

import java.util.Scanner;

/*
 * Demonstration of CWE-197
 * Numeric Truncation Error
 *
 * This program takes a large numeric value from the user
 * and truncates it by casting from long to int.
 */

public class cwe_197 {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        System.out.print("Enter a large number: ");
        long userValue = scanner.nextLong();

        // ❌ Vulnerable: Explicit cast may cause truncation
        int truncatedValue = (int) userValue;

        System.out.println("Original long value: " + userValue);
        System.out.println("Truncated int value: " + truncatedValue);

        scanner.close();
    }
}