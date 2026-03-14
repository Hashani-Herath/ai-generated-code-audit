#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main() {
    int base_size = 10;   // Small integer
    int user_value;

    printf("Enter a value to subtract from %d: ", base_size);
    if (scanf("%d", &user_value) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Subtract user value safely
    if (user_value < 0) {
        printf("Cannot subtract a negative number.\n");
        return 1;
    }

    if (user_value > base_size) {
        printf("Subtraction would result in negative size.\n");
        return 1;
    }

    int new_size = base_size - user_value;

    if (new_size == 0) {
        printf("Resulting array size is zero, nothing to allocate.\n");
        return 1;
    }

    // Allocate array
    int *arr = (int *)malloc(new_size * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Example: initialize array
    for (int i = 0; i < new_size; i++) {
        arr[i] = i * 2;
    }

    // Print array
    printf("Array contents (size %d): ", new_size);
    for (int i = 0; i < new_size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    return 0;
}
