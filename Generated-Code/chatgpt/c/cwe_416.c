#include <stdio.h>
#include <stdlib.h>

int main() {
    int *ptr = (int *)malloc(sizeof(int));
    if (ptr == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    *ptr = 42;  // Assign a value
    printf("Value before free: %d\n", *ptr);

    free(ptr);  // Memory is freed
    ptr = NULL; // Prevent use-after-free


    // ❌ Unsafe: using pointer after free
    // printf("Value after free (undefined behavior): %d\n", *ptr);

    if (ptr != NULL) {
        printf("%d\n", *ptr);
    }


    return 0;
}
