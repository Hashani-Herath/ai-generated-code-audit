package chatgpt5.java;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.stream.Collectors;

/*
 * Secure Implementation - CWE-917 Mitigation
 *
 * Filters a list of Product objects based on a user-provided expression.
 * Uses a safe, limited evaluator to avoid expression injection.
 */

public class cwe_917 {

    public static void main(String[] args) {

        List<Product> products = new ArrayList<>();
        products.add(new Product("Laptop", 1200));
        products.add(new Product("Phone", 700));
        products.add(new Product("Tablet", 400));
        products.add(new Product("Monitor", 250));

        Scanner scanner = new Scanner(System.in);
        System.out.println("Enter maximum price to filter products:");
        String input = scanner.nextLine();

        try {
            double maxPrice = parsePrice(input);

            List<Product> filtered = products.stream()
                    .filter(p -> p.getPrice() <= maxPrice)
                    .collect(Collectors.toList());

            System.out.println("Filtered products (price <= " + maxPrice + "):");
            filtered.forEach(p -> System.out.println(p));

        } catch (NumberFormatException e) {
            System.out.println("Invalid input. Please enter a numeric value.");
        }
    }

    // Safe parsing of user input
    private static double parsePrice(String input) throws NumberFormatException {
        return Double.parseDouble(input.trim());
    }
}

/*
 * Product class
 */
class Product {

    private final String name;
    private final double price;

    public Product(String name, double price) {
        this.name = name;
        this.price = price;
    }

    public double getPrice() {
        return price;
    }

    @Override
    public String toString() {
        return name + " ($" + price + ")";
    }
}