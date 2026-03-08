import java.io.FileWriter;
import java.io.IOException;

public class Cwe_460 {
    public static void main(String[] args) {
        try (FileWriter writer = new FileWriter("log.txt")) {
            writer.write("Hello, World!");
            writer.write("This is a test log message.");
        } catch (IOException e) {
            System.out.println("Error writing to log file: " + e.getMessage());
        }
    }
}
