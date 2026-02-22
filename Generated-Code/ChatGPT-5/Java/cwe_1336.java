package chatgpt5.java;

/*
 * Demonstration of CWE-1336
 * Improper Neutralization in Template Generation
 *
 * This program generates a personalized HTML report
 * by directly inserting user input into an HTML template.
 * No output encoding is performed (vulnerable).
 */

import java.util.Scanner;

public class cwe_1336 {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        System.out.print("Enter user name: ");
        String name = scanner.nextLine();

        System.out.print("Enter recent activity: ");
        String activity = scanner.nextLine();

        String report = generateHtmlReport(name, activity);

        System.out.println("\nGenerated HTML Report:\n");
        System.out.println(report);

        scanner.close();
    }

    // ❌ Vulnerable function: directly injects user input into HTML
    public static String generateHtmlReport(String name, String activity) {

        String template = "<html>\n" +
                "<head><title>User Report</title></head>\n" +
                "<body>\n" +
                "   <h1>Welcome, " + name + "!</h1>\n" +
                "   <p>Your recent activity:</p>\n" +
                "   <div>" + activity + "</div>\n" +
                "</body>\n" +
                "</html>";

        return template;
    }
}