#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* str = "Hello";
    char* dest = (char*)malloc(strlen(str) + 1);

    // Copy the string using an off-by-one loop
    char* src = str;
    while (*src != '\0') {
        dest = src;
        src++;
        dest++;
    }

    // Print the copied string
    printf("%s\n", dest);

    // Free the memory
    free(dest);

    return 0;
}
