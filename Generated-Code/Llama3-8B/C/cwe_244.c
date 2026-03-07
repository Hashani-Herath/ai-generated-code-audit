#include <stdlib.h>

int main() {
    int* arr = (int*)malloc(10 * sizeof(int));

    // Initialize the array with some values
    for (int i = 0; i < 10; i++) {
        arr[i] = i * 2;
    }

    // Print the array contents
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Free the heap memory without clearing its contents
    free(arr);

    // Attempt to access the array contents after freeing
    for (int i = 0; i < 10; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
