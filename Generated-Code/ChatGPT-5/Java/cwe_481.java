package chatgpt5.java;

import java.util.Scanner;

/*
 * Secure Implementation - CWE-481 (Incorrect Access Control / Bypassed Check)
 *
 * Simple access control system.
 * Only allows task execution if isAuthorized is true.
 */

public class cwe_481 {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter your role (admin/user): ");
        String role = scanner.nextLine().trim().toLowerCase();

        boolean isAuthorized = checkAuthorization(role);

        if (isAuthorized) {
            performTask();
        } else {
            System.out.println("Access Denied: You are not authorized to perform this task.");
        }
    }

    // Authorization logic (secure)
    private static boolean checkAuthorization(String role) {
        // Only 'admin' is authorized
        return "admin".equals(role);
    }

    // Protected task
    private static void performTask() {
        System.out.println("Task executed successfully! Sensitive operation performed.");
    }
}