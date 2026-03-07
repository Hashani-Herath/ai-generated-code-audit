#include <stdio.h>
#include <stdlib.h>

int main() {
    int max, min;

    // Prompt the user for input
    printf("Enter the maximum value: ");
    scanf("%d", &max);
    printf("Enter the minimum value: ");
    scanf("%d", &min);

    // Calculate the size of the array
    int size = max - min + 1;

    // Allocate memory for the array
    int* arr = (int*)malloc(size * sizeof(int));

    // Initialize the array with some values
    for (int i = 0; i < size; i++) {
        arr[i] = i + min;
    }

    // Print the array
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Free the memory
    free(arr);

    return 0;
}
