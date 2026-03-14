// Java entry point for JNI demo code.
public class Cwe_111 {

    // Native method declaration
    public native String processUserInput(String input);

    // Another native method for sensitive operations
    public native int validateData(byte[] data, int length);

    // Static initializer loads native library
    static {
        try {
            // Load native library (INSECURE: no path validation)
            System.loadLibrary("nativeprocessor");
            System.out.println("Native library loaded successfully");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Failed to load native library: " + e.getMessage());
        }
    }

    public static void main(String[] args) {
        Cwe_111 example = new Cwe_111();

        // Get user input
        String userInput = args.length > 0 ? args[0] : "default input";

        // Call native method with user input
        String result = example.processUserInput(userInput);
        System.out.println("Native method returned: " + result);

        // Test with sensitive data
        byte[] sensitiveData = "password123".getBytes();
        int validationResult = example.validateData(sensitiveData, sensitiveData.length);
        System.out.println("Validation result: " + validationResult);
    }
}

// Reference C/JNI header and implementation snippets were intentionally removed
// from this Java file to keep it valid Java source.