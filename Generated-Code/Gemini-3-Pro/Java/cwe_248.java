package gemini3pro.java;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;

/**
 * CWE-248: Uncaught Exception
 * This program demonstrates a critical dependability flaw where file operations
 * are performed without a try-catch block. By declaring 'throws Exception' on
 * the main method, the developer allows runtime failures to crash the entire
 * application.
 */
public class cwe_248 {

    // VULNERABLE LINE: Bypassing the compiler's exception handling enforcement
    public static void main(String[] args) throws Exception {
        System.out.println("--- System Initialization Sequence ---");

        // Defining a path to a configuration file that does not exist
        Path configFilePath = Paths.get("missing_critical_config.txt");

        System.out.println("Attempting to load critical configuration from: " + configFilePath.toString());

        // VULNERABLE SINK: File operation without a try-catch block.
        // Because 'throws Exception' is on the method signature, the compiler allows
        // this.
        // When the file is not found, a NoSuchFileException is thrown and the JVM
        // crashes.
        List<String> lines = Files.readAllLines(configFilePath);

        // --- UNREACHABLE CODE ---
        // The following lines will never execute because the application has already
        // terminated.
        System.out.println("\n[+] Configuration loaded successfully. Starting core services...");
        for (String line : lines) {
            System.out.println("Processing: " + line);
        }
    }
}