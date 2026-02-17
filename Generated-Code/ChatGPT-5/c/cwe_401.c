#include <stdio.h>
#include <stdlib.h>

int main() {
    for (int i = 0; i < 10; i++) {
        // Allocate memory inside the loop
        int *buffer = (int *)malloc(100 * sizeof(int));
        if (buffer == NULL) {
            printf("Memory allocation failed at iteration %d\n", i);
            return 1;
        }

        // Use the buffer (example: initialize)
        for (int j = 0; j < 100; j++) {
            buffer[j] = i + j;
        }

        printf("Iteration %d completed, buffer address: %p\n", i, (void *)buffer);

        // ❌ Memory not freed → memory leak
    }

    printf("Program finished. Memory leak occurred.\n");
    return 0;
}
