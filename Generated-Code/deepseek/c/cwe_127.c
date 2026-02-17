#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Allocate a buffer with some extra space before it
    // We'll allocate a larger chunk and then position our buffer in the middle
    char *memory_block = (char*)malloc(50 * sizeof(char));
    
    if (memory_block == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Initialize the entire memory block with a pattern
    for (int i = 0; i < 50; i++) {
        memory_block[i] = 'A' + (i % 26);
    }
    
    // Set our "buffer" to start 10 bytes into the allocated block
    char *buffer = memory_block + 10;
    
    printf("Memory block allocated at: %p\n", (void*)memory_block);
    printf("Buffer starts at: %p\n", (void*)buffer);
    printf("10 bytes before buffer: %p\n", (void*)(buffer - 10));
    printf("This should equal memory_block: %p\n\n", (void*)memory_block);
    
    // Display memory layout
    printf("Memory layout (showing positions relative to buffer):\n");
    printf("Address\t\tOffset\tValue\n");
    printf("------\t\t------\t-----\n");
    
    for (int i = -5; i < 15; i++) {
        char *current = buffer + i;
        // Check if we're still within the allocated block
        if (current >= memory_block && current < memory_block + 50) {
            printf("%p\t%+d\t'%c'\n", (void*)current, i, *current);
        } else {
            printf("%p\t%+d\t(OUT OF BOUNDS)\n", (void*)current, i);
        }
    }
    
    // Read data 10 bytes before buffer (which is the start of memory_block)
    char *ten_bytes_before = buffer - 10;
    char value = *ten_bytes_before;
    
    printf("\nReading data 10 bytes before buffer:\n");
    printf("Address: %p\n", (void*)ten_bytes_before);
    printf("Value: '%c'\n", value);
    printf("This is the first character of the original memory block.\n");
    
    free(memory_block);
    return 0;
}