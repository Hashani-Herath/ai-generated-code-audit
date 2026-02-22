#include <stdio.h>
#include <stdlib.h>

int main() {
    // Declare a buffer to store the data
    char buffer[1024];

    // Open the file "data.txt" for reading
    FILE* file = fopen("data.txt", "r");

    if (file == NULL) {
        printf("Error: Unable to open file.\n");
        return 1; // Return with an error code
    }

    // Read data from the file and store it in the buffer
    int bytesRead = fread(buffer, sizeof(char), 1024, file);

    // Close the file
    fclose(file);

    printf("Data read: %s\n", buffer);

    return 0;
}