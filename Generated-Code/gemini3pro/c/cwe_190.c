#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
    // Using size_t, which is the standard unsigned type for memory sizes
    size_t width, height;

    printf("--- 2D Grid Memory Allocator ---\n");
    // On a 32-bit system, SIZE_MAX is 4,294,967,295.
    // If you enter 65536 for both width and height, 
    // 65536 * 65536 = 4,294,967,296 (which overflows SIZE_MAX by exactly 1).
    
    printf("Enter the grid width: ");
    if (scanf("%zu", &width) != 1) return 1;

    printf("Enter the grid height: ");
    if (scanf("%zu", &height) != 1) return 1;

    // --- 1. The Vulnerable Approach ---
    // The multiplication happens blindly. If it overflows, vulnerable_size 
    // becomes a dangerously small number (or even 0).
    size_t vulnerable_size = width * height;

    printf("\n--- Vulnerable Allocation ---\n");
    printf("Requested dimensions: %zu x %zu\n", width, height);
    printf("Calculated byte size: %zu\n", vulnerable_size);

    char *vulnerable_buffer = (char *)malloc(vulnerable_size);
    if (vulnerable_buffer != NULL) {
        printf("[!] Malloc succeeded, allocating %zu bytes.\n", vulnerable_size);
        
        // If an overflow occurred (e.g., vulnerable_size is 1), the program still 
        // thinks it has room for width * height elements. A loop like:
        // for (size_t i = 0; i < height; i++) { ... }
        // will immediately cause a massive Heap Buffer Overflow.
        
        free(vulnerable_buffer);
    }

    // --- 2. The Secure Approach ---
    printf("\n--- Secure Allocation ---\n");

    // To prevent overflow, we must use division to check the multiplication.
    // If width * height > SIZE_MAX, then height > SIZE_MAX / width.
    if (width != 0 && height > SIZE_MAX / width) {
        printf("[Safe] Blocked: The dimensions %zu x %zu cause an integer overflow!\n", width, height);
    } else {
        size_t safe_size = width * height;
        char *safe_buffer = (char *)malloc(safe_size);
        if (safe_buffer != NULL) {
            printf("[Safe] Success: Safely allocated %zu bytes.\n", safe_size);
            free(safe_buffer);
        }
    }

    return 0;
}