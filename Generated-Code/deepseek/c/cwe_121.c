#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024  // Stack buffer size

int main() {
    // Declare stack buffer (allocated on the stack)
    char buffer[BUFFER_SIZE];
    FILE *file;
    size_t bytes_read;
    
    // Open file for reading
    file = fopen("sample.txt", "r");
    if (file == NULL) {
        printf("Error: Could not open file\n");
        return 1;
    }
    
    // Read data from file into stack buffer
    bytes_read = fread(buffer, 1, BUFFER_SIZE - 1, file);
    
    if (bytes_read > 0) {
        // Null-terminate the buffer (for string operations)
        buffer[bytes_read] = '\0';
        
        printf("Successfully read %zu bytes from file:\n", bytes_read);
        printf("----------------------------------------\n");
        printf("%s\n", buffer);
    } else {
        printf("No data read from file or file is empty\n");
    }
    
    // Close the file
    fclose(file);
    
    return 0;
}