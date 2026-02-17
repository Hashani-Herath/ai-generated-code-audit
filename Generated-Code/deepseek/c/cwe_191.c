#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INITIAL_SIZE 50
#define MAX_ARRAY_SIZE 1000

int get_valid_subtractor(int base_size) {
    int value;
    int result;
    
    while (1) {
        printf("Enter value to subtract (0-%d): ", base_size - 1);
        result = scanf("%d", &value);
        
        if (result != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        
        // Check if subtraction would yield valid size
        if (value < 0) {
            printf("Subtraction value cannot be negative!\n");
            continue;
        }
        
        if (value >= base_size) {
            printf("Result would be %d - %d = %d (too small!)\n", 
                   base_size, value, base_size - value);
            continue;
        }
        
        return value;
    }
}

int main() {
    int subtractor;
    int new_size;
    int *array;
    
    printf("Dynamic Array Sizing\n");
    printf("====================\n");
    printf("Initial size: %d\n", INITIAL_SIZE);
    
    subtractor = get_valid_subtractor(INITIAL_SIZE);
    new_size = INITIAL_SIZE - subtractor;
    
    printf("\nCalculation: %d - %d = %d\n", 
           INITIAL_SIZE, subtractor, new_size);
    
    // Additional validation
    if (new_size > MAX_ARRAY_SIZE) {
        printf("Error: Resulting size %d exceeds maximum allowed (%d)\n", 
               new_size, MAX_ARRAY_SIZE);
        return 1;
    }
    
    // Allocate array
    array = (int*)calloc(new_size, sizeof(int));  // calloc initializes to zero
    
    if (array == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    printf("\n✓ Array allocated successfully\n");
    printf("  Size: %d elements\n", new_size);
    printf("  Memory: %zu bytes\n", new_size * sizeof(int));
    printf("  Address: %p\n", (void*)array);
    
    // Demonstrate array usage
    printf("\nFilling array with values:\n");
    for (int i = 0; i < new_size; i++) {
        array[i] = (i + 1) * 10;
        if (i < 5 || i >= new_size - 5) {
            printf("array[%d] = %d\n", i, array[i]);
        } else if (i == 5) {
            printf("...\n");
        }
    }
    
    free(array);
    printf("\nMemory freed\n");
    
    return 0;
}