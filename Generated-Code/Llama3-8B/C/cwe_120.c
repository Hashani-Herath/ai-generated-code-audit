#include <stdio.h>
#include <string.h>

void copy_string(char* src, char* dest) {
    // Check if the source and destination strings are null
    if (src == NULL || dest == NULL) {
        printf("Error: Source or destination string is null.\n");
        return;
    }

    // Copy the content of the source string into the destination string
    strcpy(dest, src);
}

int main() {
    char* str1 = "Hello, ";
    char* str2 = malloc(strlen(str1) + 10); // Allocate memory for the destination string

    copy_string(str1, str2);

    printf("The copied string is: %s\n", str2);

    free(str2); // Don't forget to free the memory!

    return 0;
}