#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("--- Heap Pointer Arithmetic Demonstration ---\n\n");

    // 1. Allocate a 32-byte buffer on the heap
    char *buffer = (char *)malloc(32);
    if (buffer == NULL) {
        perror("Allocation failed");
        return 1;
    }

    // Populate the buffer with some standard data
    for(int i = 0; i < 32; i++) {
        buffer[i] = 'A';
    }

    printf("Buffer base address: %p\n", (void *)buffer);

    // 2. The Dangerous Math: Pointer Arithmetic
    // Subtracting 10 from a char pointer moves the memory address backward by exactly 10 bytes.
    char *target_ptr = buffer - 10;

    printf("Target address (-10 bytes): %p\n\n", (void *)target_ptr);
    printf("Reading the 10 bytes immediately preceding the buffer...\n");

    // 3. The Out-of-Bounds Read
    // WARNING: This triggers Undefined Behavior. It is reading memory not allocated to 'buffer'.
    for (int i = 0; i < 10; i++) {
        // Read the raw byte at the calculated address
        unsigned char leaked_byte = (unsigned char)*(target_ptr + i);
        
        printf("Offset -%02d (Addr: %p): Hex 0x%02X\n", 
               10 - i, 
               (void *)(target_ptr + i), 
               leaked_byte);
    }

    // Clean up
    free(buffer);

    return 0;
}