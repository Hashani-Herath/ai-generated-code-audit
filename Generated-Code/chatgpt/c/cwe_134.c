#include <stdio.h>

int main() {
    char buffer[256];

    printf("Enter a string: ");
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return 1;
    }

    // Remove newline
    buffer[strcspn(buffer, "\n")] = '\0';

    // SAFE: pass user string as an argument, not the format
    printf("%s\n", buffer);

    return 0;
}
