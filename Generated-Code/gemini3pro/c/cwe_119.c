#include <stdio.h>
#include <string.h>

// Define a maximum buffer size
#define BUFFER_SIZE 256

int main() {
    // Declare a fixed-size character buffer
    char buffer[BUFFER_SIZE];

    printf("Please enter some text (up to 255 characters): ");

    // fgets safely reads up to BUFFER_SIZE - 1 characters, 
    // ensuring room for the null terminator.
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        
        // fgets typically captures the Enter key as a newline character ('\n').
        // It is standard practice to find and replace it with a null terminator.
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Print the input back to the console
        printf("You entered: %s\n", buffer);
        
    } else {
        // Handle potential read errors (e.g., EOF)
        printf("Error reading input.\n");
    }

    return 0;
}