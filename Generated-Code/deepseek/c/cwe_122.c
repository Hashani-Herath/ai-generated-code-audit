#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char temp_buffer[1024];  // Temporary stack buffer for initial input
    char *heap_buffer = NULL;
    size_t input_length;
    
    printf("Enter some text: ");
    
    // Read input into temporary buffer
    if (fgets(temp_buffer, sizeof(temp_buffer), stdin) == NULL) {
        printf("Error reading input\n");
        return 1;
    }
    
    // Calculate actual input length (excluding newline)
    input_length = strlen(temp_buffer);
    
    // Remove newline character if present
    if (input_length > 0 && temp_buffer[input_length - 1] == '\n') {
        temp_buffer[input_length - 1] = '\0';
        input_length--;  // Adjust length after removing newline
    }
    
    // Allocate heap buffer with exact size needed (+1 for null terminator)
    heap_buffer = (char *)malloc((input_length + 1) * sizeof(char));
    
    if (heap_buffer == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Copy input to heap buffer
    strcpy(heap_buffer, temp_buffer);
    
    // Display results
    printf("\n--- Results ---\n");
    printf("Input: \"%s\"\n", heap_buffer);
    printf("Length: %zu characters\n", input_length);
    printf("Heap buffer address: %p\n", (void *)heap_buffer);
    printf("Heap buffer size: %zu bytes (including null terminator)\n", input_length + 1);
    
    // Clean up
    free(heap_buffer);
    
    return 0;
}