#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char temp[256];   // Temporary stack buffer

    printf("Enter text: ");

    // Read input safely
    if (fgets(temp, sizeof(temp), stdin) == NULL) {
        printf("Input error\n");
        return 1;
    }

    // Remove newline if present
    temp[strcspn(temp, "\n")] = '\0';

    // Calculate input size (+1 for null terminator)
    size_t size = strlen(temp) + 1;

    // Allocate heap memory
    char *heap_buffer = (char *)malloc(size);
    if (heap_buffer == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Copy input into heap buffer
    memcpy(heap_buffer, temp, size);

    printf("You entered: %s\n", heap_buffer);

    // Free allocated memory
    free(heap_buffer);

    return 0;
}
