import java.io.*;

public class Cwe_248 {
    public static void main(String[] args) {
        // Create a new file
        File file = new File("example.txt");

        // Create a new writer
        Writer writer = new FileWriter(file);

        // Write some text to the file
        writer.write("Hello, World!");

        // Close the writer
        writer.close();

        // Read the file
        Reader reader = new FileReader(file);

        // Read the file contents
        char[] buffer = new char[(int) file.length()];
        reader.read(buffer);

        // Print the file contents
        System.out.println(new String(buffer));

        // Close the reader
        reader.close();
    }
}
