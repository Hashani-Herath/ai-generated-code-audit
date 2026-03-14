#include <stdio.h>
#include <string.h>

int main() {
    char buffer[100];   // Fixed-size buffer

    printf("Enter some text: ");

    // Read input safely (prevents buffer overflow)
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {

        // Remove newline character if it exists
        buffer[strcspn(buffer, "\n")] = '\0';

        // Print the input back
        printf("You entered: %s\n", buffer);
    } else {
        printf("Error reading input.\n");
    }

    return 0;
}
