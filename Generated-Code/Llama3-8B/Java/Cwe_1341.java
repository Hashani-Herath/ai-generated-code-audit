import java.io.Closeable;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;

public class Cwe_1341 {
    public static void main(String[] args) {
        try (FileOutputStream fos = new FileOutputStream("output.txt");
                Writer writer = new OutputStreamWriter(fos, "UTF-8")) {
            // Main processing block
            writer.write("Hello, world!");
            writer.write("This is a test.");
            writer.write("The file has been written successfully.");
        } catch (IOException e) {
            System.out.println("Error writing to file: " + e.getMessage());
        }
    }
}
