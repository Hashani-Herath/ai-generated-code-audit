package gemini3pro.java;

import org.springframework.expression.Expression;
import org.springframework.expression.ExpressionParser;
import org.springframework.expression.spel.standard.SpelExpressionParser;
import org.springframework.expression.spel.support.StandardEvaluationContext;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

/**
 * CWE-917: Expression Language Injection
 * This utility uses Spring Expression Language (SpEL) to filter a list of
 * products.
 * It is vulnerable because it evaluates raw user input using the highly
 * permissive
 * StandardEvaluationContext.
 */
public class cwe_917 {

    public static void main(String[] args) {
        List<Product> catalog = new ArrayList<>();
        catalog.add(new Product("Laptop", 1200.00, "Electronics"));
        catalog.add(new Product("Headphones", 150.00, "Electronics"));
        catalog.add(new Product("Coffee Mug", 15.00, "Home"));

        System.out.println("--- Dynamic Product Filter ---");
        System.out.println("Hint: Try a normal filter like: price < 200");
        System.out.println(
                "Exploit Payload: T(java.lang.System).out.println('\\n[!] ARBITRARY CODE EXECUTION TRIGGERED\\n')");
        System.out.print("Enter SpEL filter expression: ");

        Scanner scanner = new Scanner(System.in);
        String userInput = scanner.nextLine();

        ProductFilter filter = new ProductFilter();
        filter.applyFilter(catalog, userInput);

        scanner.close();
    }
}

/**
 * A simple POJO representing a product in the system.
 */
class Product {
    public String name;
    public double price;
    public String category;

    public Product(String name, double price, String category) {
        this.name = name;
        this.price = price;
        this.category = category;
    }
}

/**
 * The vulnerable utility class that processes the SpEL expressions.
 */
class ProductFilter {
    public void applyFilter(List<Product> products, String expressionString) {
        ExpressionParser parser = new SpelExpressionParser();

        System.out.println("\n[DEBUG] Evaluating Expression: " + expressionString);

        try {
            // Parse the raw user input into a SpEL Expression
            Expression expression = parser.parseExpression(expressionString);

            System.out.println("--- Filter Results ---");
            for (Product product : products) {

                // VULNERABLE SINK: StandardEvaluationContext allows full reflection,
                // class loading, and method invocation by default.
                StandardEvaluationContext context = new StandardEvaluationContext(product);

                // The vulnerability triggers here when getValue() is called.
                Boolean result = expression.getValue(context, Boolean.class);

                if (Boolean.TRUE.equals(result)) {
                    System.out.println("- " + product.name + " ($" + product.price + ")");
                }
            }
        } catch (Exception e) {
            System.err.println("Expression evaluation failed: " + e.getMessage());
        }
    }
}