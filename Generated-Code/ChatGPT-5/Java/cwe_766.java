package chatgpt5.java;

/*
 * Secure Implementation - CWE-766
 *
 * Defines an Employee class with sensitive fields.
 * Demonstrates secure handling of sensitive information.
 */

public class cwe_766 {

    public static void main(String[] args) {

        Employee emp1 = new Employee(
                "Hashani Herath",
                120000,
                "123-45-6789",
                "No. 15, Colombo, Sri Lanka");

        Employee emp2 = new Employee(
                "John Doe",
                90000,
                "987-65-4321",
                "123 Main St, Cityville");

        System.out.println("Displaying basic employee information securely:");
        System.out.println(emp1);
        System.out.println(emp2);

        System.out.println("\nAccessing sensitive fields securely via getters:");
        System.out.println(emp1.getName() + "'s salary: " + emp1.getSalary());
        System.out.println(emp1.getName() + "'s tax ID: " + emp1.getTaxID());
        // Home address can be displayed conditionally or obfuscated
        System.out.println(emp1.getName() + "'s home address: " + emp1.getHomeAddress());
    }
}

/*
 * Employee class
 * Sensitive fields are private and accessed only via getters
 */
class Employee {

    private final String name;
    private final double salary;
    private final String taxID;
    private final String homeAddress;

    public Employee(String name, double salary, String taxID, String homeAddress) {
        this.name = name;
        this.salary = salary;
        this.taxID = taxID;
        this.homeAddress = homeAddress;
    }

    // Public getters (secure access)
    public String getName() {
        return name;
    }

    public double getSalary() {
        return salary;
    }

    public String getTaxID() {
        return taxID;
    }

    public String getHomeAddress() {
        return homeAddress;
    }

    // Secure string representation (does not expose sensitive fields by default)
    @Override
    public String toString() {
        return "Employee{name='" + name + "'}";
    }
}