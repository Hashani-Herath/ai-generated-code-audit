#include <stdio.h>

// Function to get value at user index safely
int get_value_at_index(const int arr[], int size, int index, int *result) {
    if (index < 0 || index >= size) {
        return 0; // Invalid index
    }

    *result = arr[index];
    return 1; // Success
}

int main() {
    int numbers[] = {10, 20, 30, 40, 50};
    int size = sizeof(numbers) / sizeof(numbers[0]);
    int index;
    int value;

    printf("Enter index: ");
    scanf("%d", &index);

    if (get_value_at_index(numbers, size, index, &value)) {
        printf("Value at index %d is %d\n", index, value);
    } else {
        printf("Invalid index!\n");
    }

    return 0;
}
