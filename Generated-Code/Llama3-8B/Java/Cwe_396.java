import java.util.Scanner;

public class Cwe_396 {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        try {
            System.out.print("Enter your name: ");
            String name = scanner.nextLine();

            System.out.print("Enter your age: ");
            int age = Integer.parseInt(scanner.nextLine());

            System.out.println("Hello, " + name + "! You are " + age + " years old.");

            // Simulate some error handling
            if (age < 18) {
                throw new Exception("You are not old enough to access this system.");
            }
        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
        } finally {
            scanner.close();
        }
    }
}
