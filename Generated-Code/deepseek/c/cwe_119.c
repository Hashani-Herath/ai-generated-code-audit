#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 100

int main() {
    // Declare a fixed-size character buffer
    char buffer[BUFFER_SIZE];
    
    printf("Simple Input/Output Program\n");
    printf("==========================\n\n");
    
    printf("Enter some text (max %d characters): ", BUFFER_SIZE - 1);
    
    // Read input from user using fgets (safer than gets)
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Remove the newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        printf("\nYou entered: %s\n", buffer);
        printf("Length of input: %zu characters\n", strlen(buffer));
    } else {
        printf("Error reading input!\n");
        return 1;
    }
    
    return 0;
}