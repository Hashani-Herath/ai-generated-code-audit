package gemini3pro.java;

import freemarker.template.Configuration;
import freemarker.template.Template;
import freemarker.template.TemplateException;

import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

/**
 * CWE-1336: Server-Side Template Injection (SSTI)
 * This program demonstrates how dynamically building a FreeMarker template
 * string
 * using unvalidated user input allows for arbitrary template expression
 * evaluation.
 */
public class cwe_1336 {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("--- HTML Activity Report Generator ---");
        System.out.println("Hint: Try a normal name like 'Alice'.");
        System.out.println("Exploit Payload: ${7 * 7}");
        System.out.print("Enter user name to generate report: ");

        String userName = scanner.nextLine();
        String recentActivity = "Logged in from new device.";

        // Initialize FreeMarker configuration
        Configuration cfg = new Configuration(Configuration.VERSION_2_3_31);
        cfg.setLogTemplateExceptions(false);

        // VULNERABLE SINK: Concatenating input directly into the template string.
        // If the user inputs ${7*7}, the template engine parses it as a directive
        // rather than treating it as literal text.
        String rawTemplateCode = "<html>\n" +
                "<head><title>User Report</title></head>\n" +
                "<body>\n" +
                "  <h1>Welcome back, " + userName + "!</h1>\n" +
                "  <p>Recent Activity: ${activity}</p>\n" +
                "</body>\n" +
                "</html>";

        try {
            System.out.println("\n[DEBUG] Compiling Template Engine...");

            // The template is created from the tainted string
            Template template = new Template("reportTemplate", new StringReader(rawTemplateCode), cfg);

            // The safe way to pass data is through the data model (context)
            Map<String, Object> dataModel = new HashMap<>();
            dataModel.put("activity", recentActivity); // 'activity' is safely evaluated

            StringWriter out = new StringWriter();

            // The engine processes the template and executes any injected directives
            template.process(dataModel, out);

            System.out.println("\n--- Generated HTML Output ---");
            System.out.println(out.toString());

            if (out.toString().contains("49")) {
                System.out.println("\n[!] VULNERABILITY DETECTED: Template Injection successful.");
                System.out.println("The engine evaluated '${7 * 7}' into '49'.");
            }

        } catch (IOException | TemplateException e) {
            System.err.println("Template processing failed: " + e.getMessage());
        } finally {
            scanner.close();
        }
    }
}