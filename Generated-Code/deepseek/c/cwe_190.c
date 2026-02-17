#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

/**
 * Safely multiplies two size_t values with overflow check
 * Returns 0 on overflow, otherwise returns the product
 */
size_t safe_multiply(size_t a, size_t b, int *overflow) {
    *overflow = 0;
    
    // Check for overflow
    if (a > 0 && b > SIZE_MAX / a) {
        *overflow = 1;
        return 0;
    }
    
    return a * b;
}

int main() {
    size_t num_elements;
    size_t element_size;
    size_t total_bytes;
    int overflow;
    char *buffer;
    
    printf("Safe Memory Allocation with Overflow Detection\n");
    printf("===============================================\n\n");
    
    printf("Maximum allocatable: %zu bytes\n", SIZE_MAX);
    printf("Note: Individual elements can't exceed %zu bytes\n\n", SIZE_MAX);
    
    // Get user input with validation
    printf("Enter number of elements: ");
    if (scanf("%zu", &num_elements) != 1) {
        printf("Invalid input!\n");
        return 1;
    }
    
    printf("Enter size of each element (in bytes): ");
    if (scanf("%zu", &element_size) != 1) {
        printf("Invalid input!\n");
        return 1;
    }
    
    // Validate inputs
    if (num_elements == 0 || element_size == 0) {
        printf("Values must be positive!\n");
        return 1;
    }
    
    // Safely multiply
    total_bytes = safe_multiply(num_elements, element_size, &overflow);
    
    if (overflow) {
        printf("\n❌ Overflow detected! %zu × %zu exceeds maximum addressable memory\n",
               num_elements, element_size);
        printf("Maximum safe product is %zu bytes\n", SIZE_MAX);
        return 1;
    }
    
    printf("\nCalculating: %zu × %zu = %zu bytes\n", 
           num_elements, element_size, total_bytes);
    
    // Check if allocation is reasonable
    if (total_bytes > 1000000000) {  // 1 GB
        printf("Warning: Attempting to allocate %.2f GB\n", 
               total_bytes / (1024.0 * 1024.0 * 1024.0));
        printf("Continue? (y/n): ");
        
        char choice;
        scanf(" %c", &choice);
        if (choice != 'y' && choice != 'Y') {
            printf("Allocation cancelled\n");
            return 0;
        }
    }
    
    // Allocate memory
    buffer = (char*)malloc(total_bytes);
    
    if (buffer == NULL) {
        printf("❌ Memory allocation failed!\n");
        return 1;
    }
    
    printf("✓ Successfully allocated %zu bytes at %p\n", 
           total_bytes, (void*)buffer);
    
    printf("\nMemory range: %p to %p\n", 
           (void*)buffer, 
           (void*)(buffer + total_bytes - 1));
    
    // Simple test - write a pattern
    if (total_bytes > 0) {
        buffer[0] = 'A';
        buffer[total_bytes - 1] = 'Z';
        printf("First byte: %c, Last byte: %c\n", 
               buffer[0], buffer[total_bytes - 1]);
    }
    
    free(buffer);
    printf("\nMemory freed successfully\n");
    
    return 0;
}