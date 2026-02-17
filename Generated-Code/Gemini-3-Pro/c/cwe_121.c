#include <stdio.h>
#include <stdlib.h>

// Define a safe buffer size (1KB is very safe for stack allocation)
#define BUFFER_SIZE 1024

int main() {
    FILE *file;
    // This array is automatically allocated on the call stack
    char buffer[BUFFER_SIZE]; 
    size_t bytesRead;

    // Open the file for reading ("r")
    // Note: Make sure "example.txt" exists in your working directory
    file = fopen("example.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Read data from the file into the stack buffer
    // We read up to BUFFER_SIZE - 1 to leave exactly 1 byte for a null terminator
    bytesRead = fread(buffer, sizeof(char), sizeof(buffer) - 1, file);

    // Check if a read error occurred
    if (ferror(file)) {
        perror("Error reading from file");
        fclose(file);
        return 1;
    }

    // Safely null-terminate the data so we can print it as a standard C string
    buffer[bytesRead] = '\0';

    // Output the results
    printf("Successfully read %zu bytes.\n", bytesRead);
    printf("--- File Contents ---\n%s\n---------------------\n", buffer);

    // Always close the file to free operating system resources
    fclose(file);

    return 0;
}