#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {
    size_t count;

    printf("--- Array Allocator ---\n");
    printf("Enter the number of integers you want to allocate: ");
    
    // Read an unsigned size_t value from the user
    if (scanf("%zu", &count) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // --- 1. The Vulnerable Approach ---
    // If a user enters a massive number (e.g., 1073741825 on a 32-bit system),
    // count * 4 (sizeof int) will overflow the maximum value of size_t.
    // The multiplication wraps around to exactly 4.
    size_t calculated_size = count * sizeof(int);
    
    printf("\n[Unsafe] You requested %zu integers.\n", count);
    printf("[Unsafe] Malloc will attempt to allocate %zu bytes.\n", calculated_size);

    int *vulnerable_buffer = (int *)malloc(calculated_size);
    if (vulnerable_buffer != NULL) {
        // If the overflow occurred, malloc(4) succeeds and allocates 4 bytes.
        // But the program still thinks it has room for 1,073,741,825 integers!
        // Writing to vulnerable_buffer[1], vulnerable_buffer[2], etc., will immediately
        // corrupt the heap.
        
        // (We free it immediately here to prevent actual system crashes)
        free(vulnerable_buffer); 
    }

    // --- 2. The Secure Approach ---
    printf("\n--- Secure Allocation ---\n");

    // Option A: Manual Overflow Check
    // We divide SIZE_MAX by the size of the type to find the absolute maximum count.
    if (count > SIZE_MAX / sizeof(int)) {
        printf("[Safe] Blocked: The requested count would cause an allocation overflow.\n");
    } else {
        printf("[Safe] Manual Check Passed. Safe to use malloc.\n");
    }

    // Option B: The Best Practice (calloc)
    // calloc takes the count and the size as separate arguments. 
    // Internally, modern implementations of calloc safely check for multiplication 
    // overflow before allocating, returning NULL if an overflow is detected.
    int *secure_buffer = (int *)calloc(count, sizeof(int));
    
    if (secure_buffer == NULL) {
        printf("[Safe] Calloc Blocked: Allocation failed or overflow prevented.\n");
    } else {
        printf("[Safe] Calloc Success: Memory safely allocated and zeroed out.\n");
        free(secure_buffer);
    }

    return 0;
}