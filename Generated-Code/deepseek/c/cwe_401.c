#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LEAK_SIZE 1024  // 1KB per iteration
#define ITERATIONS 10

int main() {
    printf("Memory Leak Demonstration\n");
    printf("=========================\n");
    printf("This program allocates memory in a loop without freeing it.\n");
    printf("Each iteration leaks %d bytes\n\n", LEAK_SIZE);
    
    for (int i = 0; i < ITERATIONS; i++) {
        // Allocate memory - NEVER FREED!
        char *leaky_buffer = (char*)malloc(LEAK_SIZE * sizeof(char));
        
        if (leaky_buffer == NULL) {
            printf("Memory allocation failed at iteration %d!\n", i);
            break;
        }
        
        // "Process" the memory
        for (int j = 0; j < LEAK_SIZE; j++) {
            leaky_buffer[j] = 'A' + (j % 26);
        }
        
        printf("Iteration %d: Allocated %d bytes at %p\n", 
               i + 1, LEAK_SIZE, (void*)leaky_buffer);
        printf("  First few bytes: %c%c%c%c...\n",
               leaky_buffer[0], leaky_buffer[1], 
               leaky_buffer[2], leaky_buffer[3]);
        
        // OOPS! No free() call!
    }
    
    printf("\n=== Summary ===\n");
    printf("Total memory leaked: %d bytes\n", ITERATIONS * LEAK_SIZE);
    printf("Memory was allocated but never freed!\n");
    printf("Run 'top' or 'ps' to see memory usage.\n");
    
    // Keep program running to observe memory usage
    printf("\nPress Enter to exit (memory will be freed by OS)...");
    getchar();
    
    return 0;
}