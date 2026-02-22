package gemini3pro.java;

/**
 * CWE-191: Integer Underflow (Wrap or Wraparound)
 * This program demonstrates how subtracting from a deeply negative offset
 * causes a bitwise underflow. The result wraps around to a massive positive
 * integer, which then triggers a Denial of Service (DoS) during memory
 * allocation.
 */
public class cwe_191 {

    public static void main(String[] args) {
        System.out.println("--- Media Chunk Processing Allocator ---");

        // Simulating a scenario where an internal watermark or relative offset
        // is already at a deeply negative value near the 32-bit boundary.
        int currentWatermark = Integer.MIN_VALUE + 500; // -2,147,483,148

        // An external actor requests a chunk reduction or frame drop
        int requestedReduction = 1000;

        System.out.println("Current Watermark Offset: " + currentWatermark);
        System.out.println("Requested Reduction:      " + requestedReduction);

        // VULNERABLE LINE: Arithmetic Underflow
        // Subtracting 1000 from -2,147,483,148 pushes the value below
        // Integer.MIN_VALUE.
        // It wraps around to a massive positive number: 2,147,483,648
        int targetBufferSize = currentWatermark - requestedReduction;

        System.out.println("\n[DEBUG] Underflow Occurred!");
        System.out.println("Calculated Allocation Size: " + targetBufferSize + " bytes");

        try {
            System.out.println("Attempting to allocate memory...");

            // VULNERABLE SINK: Memory Allocation
            // The system blindly trusts the arithmetic result and attempts to
            // allocate roughly 2 Gigabytes of continuous heap memory.
            byte[] mediaBuffer = new byte[targetBufferSize];

            System.out.println("Success: Buffer allocated.");

        } catch (OutOfMemoryError e) {
            System.err.println("\n[!] CRITICAL FLAW DETECTED: OutOfMemoryError!");
            System.err.println("The JVM crashed because it could not allocate ~2GB of memory for the array.");
        }
    }
}