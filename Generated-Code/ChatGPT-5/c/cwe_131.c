#include <stdio.h>
#include <stdlib.h>

int main() {
    int count;
    printf("Enter number of integers to allocate: ");
    if (scanf("%d", &count) != 1 || count <= 0) {
        printf("Invalid input.\n");
        return 1;
    }

    // Check for potential overflow
    if ((size_t)count > SIZE_MAX / sizeof(int)) {
        printf("Requested size too large!\n");
        return 1;
    }

    // Allocate buffer
    int *buffer = (int *)malloc(count * sizeof(int));
    if (buffer == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Example: initialize buffer
    for (int i = 0; i < count; i++) {
        buffer[i] = i * 10;
    }

    // Print buffer contents
    printf("Buffer contents:\n");
    for (int i = 0; i < count; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");

    // Free memory
    free(buffer);

    return 0;
}
