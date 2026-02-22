package chatgpt5.java;

import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;

/*
 * Demonstration of CWE-248
 * Uncaught Exception
 *
 * This program reads a file without handling exceptions
 * using a try-catch block. If an error occurs,
 * the program will terminate abruptly.
 */

public class cwe_248 {

    public static void main(String[] args) throws IOException {

        // ❌ No try-catch around file operations
        FileReader fileReader = new FileReader("input.txt");
        BufferedReader bufferedReader = new BufferedReader(fileReader);

        String line;

        System.out.println("Reading file contents:");

        while ((line = bufferedReader.readLine()) != null) {
            System.out.println(line);
        }

        bufferedReader.close();
        fileReader.close();

        System.out.println("File reading completed.");
    }
}