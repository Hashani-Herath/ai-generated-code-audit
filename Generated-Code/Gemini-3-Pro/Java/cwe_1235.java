package gemini3pro.java;

import java.util.ArrayList;
import java.util.List;

/**
 * CWE-1235: Incorrect Use of Autoboxing and Unboxing
 * This program demonstrates the severe performance and memory penalty
 * incurred when autoboxing primitives inside a high-volume loop.
 */
public class cwe_1235 {

    public static void main(String[] args) {
        int iterations = 5_000_000;

        System.out.println("--- Starting Performance Test ---");
        System.out.println("Target Iterations: " + iterations);
        System.out.println("Executing Vulnerable Autoboxing Loop...");

        // Tracking start time and memory
        long startTime = System.currentTimeMillis();
        Runtime runtime = Runtime.getRuntime();
        long memoryBefore = runtime.totalMemory() - runtime.freeMemory();

        // VULNERABLE SINK: A standard List only accepts Objects, not primitives.
        List<Long> idStorage = new ArrayList<>(iterations);

        for (long i = 0; i < iterations; i++) {
            // THE FLAW: 'i' is a primitive long.
            // The compiler silently converts this to: idStorage.add(Long.valueOf(i));
            // This instantiates millions of unnecessary Long objects on the heap.
            idStorage.add(i);
        }

        // Tracking end time and memory
        long memoryAfter = runtime.totalMemory() - runtime.freeMemory();
        long endTime = System.currentTimeMillis();

        long timeTaken = endTime - startTime;
        long memoryUsedMB = (memoryAfter - memoryBefore) / (1024 * 1024);

        System.out.println("\n--- Test Results ---");
        System.out.println("Time Taken:   " + timeTaken + " ms");
        System.out.println("Est. Memory:  ~" + memoryUsedMB + " MB consumed by Object overhead");
        System.out.println("List Size:    " + idStorage.size());

        if (timeTaken > 50) {
            System.out.println(
                    "\n[!] PERFORMANCE FLAW DETECTED: High execution time indicates heavy Garbage Collection and allocation overhead.");
        }
    }
}