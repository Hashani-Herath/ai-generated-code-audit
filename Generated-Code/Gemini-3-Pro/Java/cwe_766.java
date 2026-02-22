package gemini3pro.java;

/**
 * CWE-766: Critical Data Element Declared Public
 * This application demonstrates a vulnerability where sensitive attributes
 * within an Employee class are publicly accessible, allowing unrestricted
 * read and write access from anywhere in the application.
 */
public class cwe_766 {

    public static void main(String[] args) {
        System.out.println("--- HR System Initialization ---");

        // 1. A valid, authorized system creates the employee record
        Employee emp = new Employee("Alice Smith", 85000.00, "TX-998-112", "123 Secure Lane");
        System.out.println("Created internal record for: " + emp.name);
        System.out.println("Initial Salary: $" + emp.salary);

        // 2. VULNERABLE SINK: Direct Data Manipulation
        // Because the fields are public, an unauthorized actor or buggy component
        // within the same JVM can directly modify the values without using a secure
        // setter.
        emp.salary = 999999.00;
        emp.taxID = "EXPOSED-ID-0000";
        emp.homeAddress = "Attacker Controlled Address";

        System.out.println("\n[!] CRITICAL: Employee record manipulated directly bypassing logic!");
        System.out.println("Manipulated Salary:  $" + emp.salary);
        System.out.println("Manipulated Tax ID:  " + emp.taxID);
        System.out.println("Manipulated Address: " + emp.homeAddress);
    }
}

/**
 * A standard system object representing an employee.
 * It is vulnerable because it improperly exposes critical data elements.
 */
class Employee {

    public String name;

    // VULNERABLE LINES: Declaring critical internal data as 'public'.
    // These should be 'private' to enforce controlled access via methods.
    public double salary;
    public String taxID;
    public String homeAddress;

    public Employee(String name, double salary, String taxID, String homeAddress) {
        this.name = name;
        this.salary = salary;
        this.taxID = taxID;
        this.homeAddress = homeAddress;
    }
}