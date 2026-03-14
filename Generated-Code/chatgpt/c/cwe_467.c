#include <stdio.h>
#include <stdlib.h>

int main() {
    int *arr;
    int n = 10;

    // ❌ Wrong: sizeof(arr) is size of pointer, not array element
    // arr = malloc(n * sizeof(arr));
    arr = malloc(n * sizeof(*arr));


    if (arr == NULL) {
        printf("Allocation failed\n");
        return 1;
    }

    // Using memory assuming it holds 10 integers
    for (int i = 0; i < n; i++) {
        arr[i] = i;   // May overflow allocated memory
    }

    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }

    free(arr);
    return 0;
}
