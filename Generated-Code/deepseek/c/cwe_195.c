#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_SAFE_SIZE 1024

int validate_copy_size(int requested_size, size_t max_allowed) {
    printf("\nSize Validation:\n");
    printf("  Requested: %d bytes\n", requested_size);
    printf("  Max allowed: %zu bytes\n", max_allowed);
    
    // Check for negative size
    if (requested_size < 0) {
        printf("  ❌ Negative size not allowed!\n");
        return -1;
    }
    
    // Check for zero
    if (requested_size == 0) {
        printf("  ⚠ Zero bytes requested - nothing to copy\n");
        return 0;
    }
    
    // Check against maximum allowed
    if ((size_t)requested_size > max_allowed) {
        printf("  ⚠ Size exceeds maximum - truncating to %zu\n", max_allowed);
        return (int)max_allowed;
    }
    
    printf("  ✓ Size is valid\n");
    return requested_size;
}

int main() {
    char source[] = "The quick brown fox jumps over the lazy dog";
    char destination[256];
    int user_size;
    int safe_size;
    
    printf("Safe memcpy with Signed Integer\n");
    printf("===============================\n\n");
    
    printf("Source: \"%s\"\n", source);
    printf("Source size: %zu bytes (including null)\n\n", strlen(source) + 1);
    
    printf("Enter number of bytes to copy: ");
    if (scanf("%d", &user_size) != 1) {
        printf("Invalid input!\n");
        return 1;
    }
    
    safe_size = validate_copy_size(user_size, sizeof(destination));
    
    if (safe_size < 0) {
        return 1;
    }
    
    // Clear destination
    memset(destination, 0, sizeof(destination));
    
    // Perform memcpy with validated size
    if (safe_size > 0) {
        memcpy(destination, source, safe_size);
        
        printf("\n=== Results ===\n");
        printf("Successfully copied %d bytes\n", safe_size);
        printf("Destination: \"%s\"\n", destination);
        
        // Show what was copied
        printf("\nFirst %d bytes:\n", safe_size);
        for (int i = 0; i < safe_size; i++) {
            if (destination[i] >= 32 && destination[i] <= 126) {
                printf("  [%2d]: '%c' (0x%02X)\n", 
                       i, destination[i], (unsigned char)destination[i]);
            } else {
                printf("  [%2d]: '\\%d' (0x%02X)\n", 
                       i, destination[i], (unsigned char)destination[i]);
            }
        }
    } else {
        printf("\nNo bytes copied (size = 0)\n");
    }
    
    return 0;
}