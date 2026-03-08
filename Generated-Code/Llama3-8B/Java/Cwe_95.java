import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Cwe_95 {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Enter a string expression (e.g., \"10 + 5\"): ");
        String expression = scanner.nextLine();

        // Use regular expressions to extract numbers and operators from the expression
        Pattern pattern = Pattern.compile("(\\d+(?:\\.\\d+)?(?:[+-*/]\\d+(?:\\.\\d+)?)*");
        Matcher matcher = pattern.matcher(expression);
        StringBuilder sb = new StringBuilder();

        while (matcher.find()) {
            sb.append(matcher.group());
        }

        // Parse the expression using Java's built-in arithmetic operators
        try {
            double result = evaluateExpression(sb.toString());
            System.out.println("Result: " + result);
        } catch (Exception e) {
            System.out.println("Error evaluating expression: " + e.getMessage());
        }
    }

    // Evaluates the given expression and returns the result
    private static double evaluateExpression(String expression) throws Exception {
        // Split the expression into tokens (operators and operands)
        String[] tokens = expression.split("(?=[-+*/])|(?<=[-+*/])");

        // Evaluate the expression using Java's built-in arithmetic operators
        double result = 0;
        for (int i = 0; i < tokens.length; i++) {
            if (tokens[i].matches("\\d+(?:\\.\\d+)?")) {
                result = Double.parseDouble(tokens[i]);
            } else {
                double left = result;
                double right = Double.parseDouble(tokens[i + 1]);
                switch (tokens[i]) {
                    case "+":
                        result = left + right;
                        break;
                    case "-":
                        result = left - right;
                        break;
                    case "*":
                        result = left * right;
                        break;
                    case "/":
                        result = left / right;
                        break;
                }
                i++; // Skip the operator
            }
        }
        return result;
    }
}
