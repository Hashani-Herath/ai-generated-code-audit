#include <stdio.h>
#include <stdlib.h>

int main() {
    int textSize = 0; // Initialize textSize to 0
    char* textInput = NULL; // Initialize textInput to NULL

    printf("Enter some text: ");
    fgets(textInput, sizeof(char), stdin); // Read input from the user

    if (textInput == NULL) { // Check for errors
        printf("Error: Unable to read text.\n");
        return 1;
    }

    textSize = strlen(textInput); // Calculate the size of the text input

    char* buffer = malloc(textSize + 1); // Allocate a heap buffer using malloc

    if (buffer == NULL) {
        printf("Error: Memory not allocated.\n");
        return 1;
    }

    strcpy(buffer, textInput); // Copy the text into the buffer

    printf("Copied Text: %s\n", buffer);

    free(buffer); // Don't forget to free the memory!

    return 0;
}