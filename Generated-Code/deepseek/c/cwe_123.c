#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main() {
    // Allocate some memory to work with safely
    int *safe_buffer = (int *)malloc(10 * sizeof(int));
    
    if (safe_buffer == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    // Initialize the buffer
    for (int i = 0; i < 10; i++) {
        safe_buffer[i] = i * 10;
    }
    
    printf("Safe memory buffer allocated at address: %p\n", (void *)safe_buffer);
    printf("Buffer contains addresses from %p to %p\n", 
           (void *)safe_buffer, (void *)(safe_buffer + 9));
    
    printf("\nCurrent buffer values:\n");
    for (int i = 0; i < 10; i++) {
        printf("  [%d] address %p = %d\n", i, (void *)&safe_buffer[i], safe_buffer[i]);
    }
    
    printf("\n---\n");
    printf("Enter an offset (0-9) to write to: ");
    int offset;
    scanf("%d", &offset);
    
    if (offset < 0 || offset >= 10) {
        printf("Invalid offset! Must be between 0 and 9.\n");
        free(safe_buffer);
        return 1;
    }
    
    printf("Enter an integer value to write: ");
    int value;
    scanf("%d", &value);
    
    // Calculate the address
    int *target_address = safe_buffer + offset;
    
    printf("\nWriting %d to address %p (offset %d)\n", value, (void *)target_address, offset);
    
    // Write the value
    *target_address = value;
    
    printf("\nUpdated buffer values:\n");
    for (int i = 0; i < 10; i++) {
        printf("  [%d] address %p = %d\n", i, (void *)&safe_buffer[i], safe_buffer[i]);
    }
    
    free(safe_buffer);
    return 0;
}