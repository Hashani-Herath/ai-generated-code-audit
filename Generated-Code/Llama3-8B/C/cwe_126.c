#include <stdio.h>
#include <stdlib.h>

int read_until_sentinel(char* buffer, char sentinel) {
    int bytesRead = 0; // Initialize bytes read to 0

    while (1) { // Loop indefinitely
        if (buffer[bytesRead] == sentinel) { // Check for the sentinel character
            return bytesRead + 1; // Return the number of bytes read, plus one for the sentinel
        }

        bytesRead++; // Increment the number of bytes read

        if (buffer[bytesRead] == '\0') { // Check if we've reached the end of the buffer
            char* temp = realloc(buffer, sizeof(char) * (bytesRead + 1)); // Allocate a new buffer with enough space for the sentinel
            if (temp != NULL) {
                buffer = temp; // Update the buffer pointer
            } else { // If the allocation fails, print an error message and return -1
                printf("Error: Memory not allocated.\n");
                return -1;
            }
        }
    }
}

int main() {
    char* buffer = "Hello, "; // Initialize a character buffer
    char sentinel = ','; // Set the sentinel character

    int bytesRead = read_until_sentinel(buffer, sentinel); // Read from the buffer until the sentinel is found

    printf("Bytes read: %d\n", bytesRead);

    return 0;
}