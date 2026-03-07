#include <stdio.h>
#include <string.h>

int main() {
    char src[] = "Hello, ";
    char dst[20];

    // Copy the source string into the destination string
    // using an unsafe string copy operation
    strcpy(dst, src);

    // Print the copied string
    printf("Copied string: %s\n", dst);

    return 0;
}
