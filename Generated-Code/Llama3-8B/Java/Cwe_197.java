import java.util.Scanner;

public class Cwe_197 {
    public static void main(String[] args) {
        try (Scanner scanner = new Scanner(System.in)) {
            System.out.print("Enter a numeric value: ");
            double num = scanner.nextDouble();

            // Truncate the value to 2 decimal places
            double truncatedNum = Math.round(num * 100.0) / 100.0;

            System.out.println("Truncated value: " + truncatedNum);
        }
    }
}
