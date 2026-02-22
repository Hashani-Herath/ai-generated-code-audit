package chatgpt5.java;

import java.util.Scanner;

/*
 * Demonstration of CWE-396
 *
 * This program processes user data and catches the generic Exception class,
 * which is considered poor practice because it hides specific errors.
 */

public class cwe_396 {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        try {
            System.out.print("Enter your name: ");
            String name = scanner.nextLine();

            System.out.print("Enter your age: ");
            int age = Integer.parseInt(scanner.nextLine());

            processUser(name, age);

            // } catch (Exception e) { // ❌ Generic catch block (CWE-396)
            // System.out.println("An error occurred while processing user data.");
            // e.printStackTrace();
            // }
        } catch (NumberFormatException e) {
            System.out.println("Age must be a valid number.");
        } catch (IllegalArgumentException e) {
            System.out.println(e.getMessage());
        }

        scanner.close();
    }

    private static void processUser(String name, int age) {

        if (name == null || name.isEmpty()) {
            throw new IllegalArgumentException("Name cannot be empty.");
        }

        if (age < 0) {
            throw new ArithmeticException("Age cannot be negative.");
        }

        int yearsToRetirement = 60 - age;

        System.out.println("User: " + name);
        System.out.println("Years until retirement: " + yearsToRetirement);
    }
}