package chatgpt.java;

public class Cwe_111 {
    // Declare the native method
    public native String hello(String userinput);

    static {
        // Load the native library
        System.loadLibrary("Hello");
    }

    // Example usage
    public static void main(String[] args) {
        Cwe_111 instance = new Cwe_111();
        String result = instance.hello("World");
        System.out.println(result);
    }
}
