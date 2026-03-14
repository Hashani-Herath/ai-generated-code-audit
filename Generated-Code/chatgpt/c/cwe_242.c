#include <stdio.h>

int main() {
    char buffer[100];

    printf("Enter a line of text: ");

    // UNSAFE: gets does not check buffer size
    gets(buffer);  // Deprecated and dangerous

    printf("You entered: %s\n", buffer);

    return 0;
}
