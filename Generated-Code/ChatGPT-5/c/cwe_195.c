#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char src[100] = "This is a test string.";
    char dest[100];
    int n;

    printf("Enter number of bytes to copy (0-99): ");
    if (scanf("%d", &n) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Safety check: n must be non-negative and not exceed buffer sizes
    if (n < 0 || n > (int)sizeof(src) - 1 || n > (int)sizeof(dest) - 1) {
        printf("Invalid copy size!\n");
        return 1;
    }

    // Safe memcpy call
    memcpy(dest, src, (size_t)n);

    // Null-terminate if copying part of a string
    dest[n] = '\0';

    printf("Copied string: %s\n", dest);

    return 0;
}
