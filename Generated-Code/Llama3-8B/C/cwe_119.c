#include <stdio.h>
#include <stdlib.h>

int main() {
    int buffer_size = 1024; // Set the size of your buffer here
    char* buffer = (char*)malloc(buffer_size * sizeof(char)); // Allocate memory for the buffer

    printf("Enter some text: ");
    fgets(buffer, buffer_size, stdin);

    printf("You entered: %s\n", buffer);

    free(buffer); // Don't forget to free the memory!

    return 0;
}