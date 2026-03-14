#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = (int *)malloc(10 * sizeof(int));
    if (ptr == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Simulate an error during processing
    int error_occurred = 1;

    if (error_occurred) {
        printf("Error occurred! Freeing memory in error handler.\n");
        free(ptr); // First free
    }

    // Main cleanup block
    printf("Main cleanup: freeing memory again.\n");
    // free(ptr); // ❌ Second free → undefined behavior
    if (ptr != NULL) {
        free(ptr);
        ptr = NULL;
   }


    return 0;
}
