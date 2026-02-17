#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int get_positive_count() {
    int count;
    int result;
    
    while (1) {
        printf("Enter number of integers (1-%d): ", INT_MAX / (int)sizeof(int));
        result = scanf("%d", &count);
        
        // Check for valid input
        if (result != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');  // Clear input buffer
            continue;
        }
        
        // Check for positive count
        if (count <= 0) {
            printf("Count must be positive!\n");
            continue;
        }
        
        // Check for reasonable size (optional)
        if (count > 1000000) {
            printf("Warning: Allocating %d integers (%zu bytes) is very large.\n", 
                   count, count * sizeof(int));
            printf("Continue? (y/n): ");
            
            char choice;
            scanf(" %c", &choice);
            if (choice != 'y' && choice != 'Y') {
                continue;
            }
        }
        
        return count;
    }
}

int main() {
    int *buffer = NULL;
    int count;
    
    printf("Dynamic Memory Allocation with Validation\n");
    printf("==========================================\n");
    
    // Get validated count
    count = get_positive_count();
    
    // Allocate memory
    buffer = (int*)malloc(count * sizeof(int));
    
    if (buffer == NULL) {
        printf("\n❌ Memory allocation failed!\n");
        printf("This could be due to insufficient memory or system limits.\n");
        return 1;
    }
    
    printf("\n✅ Memory allocation successful!\n");
    printf("   Allocated %d integers\n", count);
    printf("   Total size: %zu bytes\n", count * sizeof(int));
    printf("   Address range: %p to %p\n", 
           (void*)buffer, 
           (void*)((char*)buffer + count * sizeof(int) - 1));
    
    // Demonstrate usage
    printf("\nEnter values for the array:\n");
    for (int i = 0; i < count; i++) {
        printf("  buffer[%d] = ", i);
        scanf("%d", &buffer[i]);
    }
    
    printf("\nArray contents:\n");
    for (int i = 0; i < count; i++) {
        printf("  buffer[%d] = %d\n", i, buffer[i]);
    }
    
    // Free memory
    free(buffer);
    printf("\nMemory freed successfully\n");
    
    return 0;
}