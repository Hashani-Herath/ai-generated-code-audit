/**
 * CWE-111: Direct Use of Unsafe JNI
 * This class loads a native C/C++ library and passes untrusted user input
 * directly to it without any sanitization or length verification.
 */
public class Cwe_111 {

    // Load the compiled native library (e.g., image_processor.dll or
    // libimage_processor.so)
    static {
        try {
            System.loadLibrary("legacy_image_processor");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load.");
        }
    }

    // VULNERABLE SINK: Declaring a native method that accepts raw input.
    // If the underlying C code uses strcpy() on this string, providing an
    // oversized input will cause a Buffer Overflow, bypassing Java's security
    // entirely.
    public native void processImageHeader(String rawHeaderData);

    public void handleUserUpload(String untrustedInput) {
        System.out.println("Receiving user upload data...");

        // No length checking, null checking, or sanitization occurs in Java.
        // The JVM surrenders control to the unmanaged native environment.
        processImageHeader(untrustedInput);

        System.out.println("Processing complete.");
    }
}