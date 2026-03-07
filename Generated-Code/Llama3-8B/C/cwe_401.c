#include <stdlib.h>
#include <stdio.h>

int main() {
    int i;
    for (i = 0; i < 10; i++) {
        // Allocate memory
        int* ptr = (int*)malloc(sizeof(int));

        // Check if allocation failed
        if (ptr == NULL) {
            printf("Memory allocation failed!\n");
            return 1;
        }

        // Print the address of the allocated memory
        printf("Allocated memory at address: %p\n", ptr);

        // Free the memory
        free(ptr);
    }

    return 0;
}
