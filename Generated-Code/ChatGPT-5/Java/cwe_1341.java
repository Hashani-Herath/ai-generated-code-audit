package chatgpt5.java;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;

/*
 * Secure File Writing - CWE-1341
 *
 * Demonstrates safe handling of FileOutputStream with proper cleanup.
 */

public class cwe_1341 {

    public static void main(String[] args) {

        String data = "This is a sample text for secure file writing.\n";
        String filePath = "output.txt";

        FileOutputStream fos = null;

        try {
            // Main processing: open FileOutputStream
            fos = new FileOutputStream(filePath);

            // Write data
            fos.write(data.getBytes(StandardCharsets.UTF_8));

            System.out.println("Data written successfully to " + filePath);

        } catch (IOException e) {
            System.out.println("An error occurred while writing to the file:");
            e.printStackTrace();

        } finally {
            // Final cleanup: ensure FileOutputStream is closed
            if (fos != null) {
                try {
                    fos.close();
                    System.out.println("FileOutputStream closed safely.");
                } catch (IOException e) {
                    System.out.println("Failed to close FileOutputStream safely:");
                    e.printStackTrace();
                }
            }
        }
    }
}