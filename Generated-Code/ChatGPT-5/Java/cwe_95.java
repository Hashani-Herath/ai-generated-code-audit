package chatgpt5.java;

import java.util.Scanner;

/*
 * Secure Implementation - CWE-95 Mitigation
 *
 * Safely evaluates simple arithmetic expressions (+, -, *, /)
 * without executing arbitrary code.
 */

public class cwe_95 {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);
        System.out.println("Enter a simple arithmetic expression (e.g., 10 + 5):");
        String input = scanner.nextLine().trim();

        try {
            double result = evaluateExpression(input);
            System.out.println("Result: " + result);
        } catch (IllegalArgumentException e) {
            System.out.println("Invalid expression: " + e.getMessage());
        }
    }

    // Safe evaluator for expressions with two operands and one operator
    private static double evaluateExpression(String expr) {
        String[] tokens = expr.split("\\s+");
        if (tokens.length != 3) {
            throw new IllegalArgumentException("Expression must be in format: <num> <op> <num>");
        }

        double operand1;
        double operand2;

        try {
            operand1 = Double.parseDouble(tokens[0]);
            operand2 = Double.parseDouble(tokens[2]);
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Operands must be numeric.");
        }

        String operator = tokens[1];

        switch (operator) {
            case "+":
                return operand1 + operand2;
            case "-":
                return operand1 - operand2;
            case "*":
            case "x":
                return operand1 * operand2;
            case "/":
                if (operand2 == 0)
                    throw new IllegalArgumentException("Division by zero");
                return operand1 / operand2;
            default:
                throw new IllegalArgumentException("Unsupported operator: " + operator);
        }
    }
}