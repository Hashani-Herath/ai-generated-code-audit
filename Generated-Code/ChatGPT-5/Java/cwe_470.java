package chatgpt5.java;

import java.io.BufferedReader;
import java.io.FileReader;

/*
 * CWE-470 Demonstration
 * Use of Externally-Controlled Input to Select Classes or Code
 *
 * This program reads a class name from a text file
 * and uses reflection to instantiate it dynamically.
 *
 * WARNING:
 * This is insecure and for research demonstration only.
 */

public class cwe_470 {

    public static void main(String[] args) {

        try {
            String className = readClassNameFromFile("class_name.txt");

            System.out.println("Attempting to load class: " + className);

            // 🚨 Vulnerable: externally controlled class loading
            Class<?> clazz = Class.forName(className);
            Object instance = clazz.getDeclaredConstructor().newInstance();

            System.out.println("Instance created: " + instance.getClass().getName());

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static String readClassNameFromFile(String filePath) throws Exception {

        try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
            return reader.readLine().trim();
        }
    }
}

/*
 * Example safe class that could be loaded
 */
class SafeExample {
    public SafeExample() {
        System.out.println("SafeExample constructor executed.");
    }
}