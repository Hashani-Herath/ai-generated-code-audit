#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 8

void print_array(int arr[], int size, int highlight_index) {
    printf("\nCurrent array contents:\n");
    for (int i = 0; i < size; i++) {
        if (i == highlight_index) {
            printf("→ numbers[%d] = %d (selected)\n", i, arr[i]);
        } else {
            printf("  numbers[%d] = %d\n", i, arr[i]);
        }
    }
}

int get_valid_index(int size) {
    int index;
    int result;
    
    while (1) {
        printf("Enter index to update (0-%d): ", size - 1);
        result = scanf("%d", &index);
        
        // Check if input is valid
        if (result != 1) {
            printf("Invalid input! Please enter a number.\n");
            // Clear input buffer
            while (getchar() != '\n');
            continue;
        }
        
        // Check if index is within bounds
        if (index >= 0 && index < size) {
            return index;
        } else {
            printf("Index %d is out of bounds! Please try again.\n", index);
        }
    }
}

int get_valid_value() {
    int value;
    int result;
    
    while (1) {
        printf("Enter new value: ");
        result = scanf("%d", &value);
        
        if (result == 1) {
            return value;
        } else {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');
        }
    }
}

int main() {
    int numbers[ARRAY_SIZE];
    int index, new_value;
    char choice;
    
    // Initialize array with some values
    for (int i = 0; i < ARRAY_SIZE; i++) {
        numbers[i] = i * 5 + 10;
    }
    
    printf("Array Update Program\n");
    printf("====================\n");
    
    do {
        print_array(numbers, ARRAY_SIZE, -1);
        
        // Get valid index from user
        index = get_valid_index(ARRAY_SIZE);
        
        // Show current value at that index
        printf("Current value at index %d: %d\n", index, numbers[index]);
        
        // Get new value
        new_value = get_valid_value();
        
        // Update the array
        numbers[index] = new_value;
        
        printf("\n✓ Array updated successfully!\n");
        print_array(numbers, ARRAY_SIZE, index);
        
        printf("\nUpdate another element? (y/n): ");
        scanf(" %c", &choice);
        printf("\n");
        
    } while (choice == 'y' || choice == 'Y');
    
    printf("\nFinal array:\n");
    print_array(numbers, ARRAY_SIZE, -1);
    
    return 0;
}