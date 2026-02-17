#include <stdio.h>

// Function to get value at index
int get_value_at_index(int arr[], int size, int index) {
    // Check if index is valid
    if (index < 0 || index >= size) {
        printf("Error: Index %d is out of bounds (valid range: 0 to %d)\n", index, size - 1);
        return -1;  // Return error value
    }
    
    return arr[index];
}

int main() {
    int numbers[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    int size = sizeof(numbers) / sizeof(numbers[0]);
    int user_index;
    
    printf("Array contents:\n");
    for (int i = 0; i < size; i++) {
        printf("Index %d: %d\n", i, numbers[i]);
    }
    
    printf("\nEnter an index to retrieve (0-%d): ", size - 1);
    scanf("%d", &user_index);
    
    int value = get_value_at_index(numbers, size, user_index);
    
    if (value != -1) {
        printf("Value at index %d is: %d\n", user_index, value);
    }
    
    return 0;
}