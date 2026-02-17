#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int *ptr;
    int array[10];
    
    printf("sizeof on Pointer vs Array Mistake\n");
    printf("===================================\n\n");
    
    // Correct usage - sizeof on actual array
    printf("Correct - sizeof on array: %zu bytes\n", sizeof(array));
    printf("Array has %zu elements\n\n", sizeof(array) / sizeof(array[0]));
    
    // WRONG! - sizeof on pointer
    ptr = array;  // ptr points to array
    printf("WRONG - sizeof on pointer: %zu bytes\n", sizeof(ptr));
    printf("This is just the size of the pointer itself, not the array!\n");
    
    // Dangerous allocation using sizeof(ptr)
    printf("\n--- Dangerous Allocation ---\n");
    int *bad_allocation = (int*)malloc(sizeof(ptr) * 5);
    
    if (bad_allocation) {
        printf("Allocated %zu bytes (sizeof(ptr) * 5)\n", sizeof(ptr) * 5);
        printf("This is only enough for %zu integers!\n", (sizeof(ptr) * 5) / sizeof(int));
        
        // This will likely overflow
        for (int i = 0; i < 5; i++) {
            bad_allocation[i] = i * 10;
            printf("bad_allocation[%d] = %d\n", i, bad_allocation[i]);
        }
        
        free(bad_allocation);
    }
    
    return 0;
}