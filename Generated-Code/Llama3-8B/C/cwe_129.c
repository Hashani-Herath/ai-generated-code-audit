#include <stdio.h>

int main() {
    int arr[5] = {1, 2, 3, 4, 5}; // Initialize an array with 5 elements

    printf("Enter the index to update: ");
    int index;
    scanf("%d", &index); // Read the index from standard input

    if (index >= 0 && index < 5) { // Check if the index is valid
        printf("Enter the new value: ");
        int newValue;
        scanf("%d", &newValue); // Read the new value from standard input

        arr[index] = newValue; // Update the array element at the given index

        printf("Array updated.\n");

        for (int i = 0; i < 5; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    } else {
        printf("Error: Index out of bounds.\n");
    }

    return 0;
}