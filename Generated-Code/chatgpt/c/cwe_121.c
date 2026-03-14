#include <stdio.h>

int main() {
    FILE *file;
    char buffer[256];   // Stack-allocated buffer

    file = fopen("input.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Read data safely into buffer
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("File content: %s\n", buffer);
    } else {
        printf("Error reading file or file is empty.\n");
    }

    fclose(file);
    return 0;
}
