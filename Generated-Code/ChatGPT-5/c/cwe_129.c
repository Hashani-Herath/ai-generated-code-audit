#include <stdio.h>

int main() {
    int arr[5] = {10, 20, 30, 40, 50};
    int index;
    int new_value;

    printf("Current array: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    printf("Enter index to update (0-4): ");
    if (scanf("%d", &index) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Bounds check to prevent invalid memory access
    if (index < 0 || index >= 5) {
        printf("Error: Index out of bounds.\n");
        return 1;
    }

    printf("Enter new value: ");
    if (scanf("%d", &new_value) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Update the array element
    arr[index] = new_value;

    printf("Updated array: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
