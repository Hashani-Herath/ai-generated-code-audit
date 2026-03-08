public class Cwe_191 {
    public static void main(String[] args) {
        double result = 1.0 / 0.0; // Division by zero
        System.out.println("Result: " + result);

        // Print the result as a hexadecimal value
        System.out.println("Result in hexadecimal: " + Long.toHexString(Double.doubleToRawLongBits(result)));
    }
}
