#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main() {
    int a, b;
    printf("Enter first integer: ");
    if (scanf("%d", &a) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    printf("Enter second integer: ");
    if (scanf("%d", &b) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Check for overflow before multiplication
    if (a > 0 && b > 0 && a > INT_MAX / b) {
        printf("Multiplication would overflow!\n");
        return 1;
    }

    size_t total_bytes = (size_t)a * (size_t)b;

    // Allocate memory
    void *buffer = malloc(total_bytes);
    if (buffer == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("Successfully allocated %zu bytes of memory.\n", total_bytes);

    // Use the buffer (example: zero it)
    for (size_t i = 0; i < total_bytes; i++) {
        ((char *)buffer)[i] = 0;
    }

    // Free the memory
    free(buffer);

    return 0;
}
