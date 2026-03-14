#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_BUFFER_SIZE 1024

int main() {
    // Temporary stack buffer to catch the initial input
    char temp_buffer[TEMP_BUFFER_SIZE];
    
    // Pointer that will hold the address of our heap memory
    char *dynamic_buffer; 
    size_t required_size;

    printf("Enter some text (up to 1023 characters): ");

    // 1. Read input into the temporary buffer
    if (fgets(temp_buffer, sizeof(temp_buffer), stdin) != NULL) {
        
        // Strip the trailing newline character added by fgets
        size_t len = strlen(temp_buffer);
        if (len > 0 && temp_buffer[len - 1] == '\n') {
            temp_buffer[len - 1] = '\0';
            len--; // Adjust length after trimming
        }

        // 2. Calculate the exact size needed 
        // We must add 1 to the string length to accommodate the '\0' null terminator
        required_size = len + 1;

        // 3. Allocate the heap buffer using malloc
        // malloc returns a void pointer, which we cast to a char pointer
        dynamic_buffer = (char *)malloc(required_size * sizeof(char));

        // Always check if malloc succeeded (it returns NULL if the system is out of memory)
        if (dynamic_buffer == NULL) {
            perror("Memory allocation failed");
            return 1;
        }

        // 4. Copy the data from the stack buffer to the new heap buffer
        strncpy(dynamic_buffer, temp_buffer, required_size);

        // Verify the result
        printf("\n--- Results ---\n");
        printf("Heap string: %s\n", dynamic_buffer);
        printf("Bytes allocated: %zu\n", required_size);

        // 5. CRITICAL: Free the dynamically allocated memory
        // Unlike stack memory, heap memory is not cleaned up automatically.
        free(dynamic_buffer);
        
        // Best practice: Set the pointer to NULL after freeing to prevent "dangling pointers"
        dynamic_buffer = NULL; 

    } else {
        printf("Error reading input.\n");
    }

    return 0;
}