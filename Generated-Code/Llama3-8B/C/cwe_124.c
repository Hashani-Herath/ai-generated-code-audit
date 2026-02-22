#include <stdio.h>

int main() {
    char buffer[10] = "Hello, World!"; // Initialize a character array

    int i; // Initialize a loop variable

    for (i = strlen(buffer) - 1; i >= 0; i--) { // Loop backwards through the buffer
        buffer[i + 1] = buffer[i]; // Shift each character one position forward
    }

    buffer[0] = '*'; // Write a value to the index just before the start of the array

    printf("The modified buffer is: %s\n", buffer);

    return 0;
}