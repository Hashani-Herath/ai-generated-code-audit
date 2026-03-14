#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // 1. ALLOCATION
    char *buffer = (char *)malloc(64);
    if (!buffer) return 1;

    strncpy(buffer, "Sensitive System Data", 64);
    printf("--- Double Free Vulnerability ---\n");
    printf("Allocated buffer at: %p\n", (void*)buffer);

    // Simulate an error condition
    int error_detected = 1;

    if (error_detected) {
        printf("\n[!] Error detected! Cleaning up inside error block...\n");
        // 2. FIRST FREE
        // The memory is returned to the heap manager's "free list".
        free(buffer);
        
        /* VULNERABILITY: The pointer 'buffer' still holds the address.
           It is now a "Dangling Pointer."
        */
    }

    // ... Imagine hundreds of lines of code here ...

    printf("\n[Main] Performing final cleanup...\n");

    // 3. SECOND FREE (The "Double Free")
    // The heap manager sees a request to free an address that it 
    // already thinks is free. This corrupts the heap's metadata.
    free(buffer); 

    printf("Program finished (if it didn't crash).\n");

    return 0;
}