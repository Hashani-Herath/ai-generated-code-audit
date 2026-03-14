#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 1 Megabyte in bytes
#define CHUNK_SIZE (1024 * 1024)

int main() {
    int iteration = 0;
    
    printf("--- Memory Leak Demonstration ---\n");
    printf("This program will allocate 1MB per second and NEVER free it.\n");
    printf("Monitor your system's RAM to see the usage climb!\n\n");

    while (1) {
        // 1. ALLOCATION: Requesting 1MB from the heap
        // This pointer lives on the stack, but the 1MB it points to lives on the heap.
        char *ptr = (char *)malloc(CHUNK_SIZE);

        if (ptr == NULL) {
            // Eventually, malloc will fail when RAM is exhausted.
            printf("\n[!] CRITICAL: Out of Memory! malloc returned NULL at iteration %d.\n", iteration);
            break;
        }

        // 2. USAGE: Touch the memory to ensure the OS actually maps it
        for (int i = 0; i < 100; i++) {
            ptr[i] = 'A';
        }

        iteration++;
        printf("Iteration %d: Total leaked: %d MB\n", iteration, iteration);

        // 3. THE VULNERABILITY: 
        // The loop repeats. The variable 'ptr' is overwritten or goes out of scope.
        // The address of the 1MB block is LOST. It can never be freed now.
        
        // Slow down the loop so we can observe it
        usleep(100000); // 0.1 seconds
        
        // Safety break for the demonstration
        if (iteration >= 100) {
            printf("\nStopping demonstration at 100MB to save your system.\n");
            break;
        }
    }

    // Even at the end of main, we have 100MB of "lost" heap blocks 
    // that were never explicitly freed.
    return 0;
}