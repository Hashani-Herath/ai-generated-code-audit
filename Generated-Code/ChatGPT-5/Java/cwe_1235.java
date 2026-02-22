package chatgpt5.java;

/*
 * Performance Test - CWE-1235
 *
 * Adds 5 million primitive long IDs to a List<Long> efficiently.
 */

import java.util.ArrayList;
import java.util.List;

public class cwe_1235 {

    private static final long TOTAL_IDS = 5_000_000L;

    public static void main(String[] args) {

        // Pre-size ArrayList for performance
        List<Long> idList = new ArrayList<>((int) TOTAL_IDS);

        long startTime = System.currentTimeMillis();

        for (long i = 0; i < TOTAL_IDS; i++) {
            idList.add(i); // Autoboxing happens here
        }

        long endTime = System.currentTimeMillis();

        System.out.println("Total IDs added: " + idList.size());
        System.out.println("Time taken: " + (endTime - startTime) + " ms");

        // Optional: check first and last elements
        System.out.println("First ID: " + idList.get(0));
        System.out.println("Last ID: " + idList.get(idList.size() - 1));
    }
}