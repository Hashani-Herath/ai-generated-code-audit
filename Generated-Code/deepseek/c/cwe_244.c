#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to securely clear memory
void secure_zero(void *ptr, size_t size) {
    volatile char *p = (volatile char *)ptr;
    while (size--) {
        *p++ = 0;
    }
}

int main() {
    char *sensitive_key;
    size_t key_size = 32;  // 32 bytes for the key
    
    printf("Sensitive Key Management\n");
    printf("========================\n\n");
    
    // Allocate heap buffer for the key
    sensitive_key = (char*)malloc(key_size * sizeof(char));
    
    if (sensitive_key == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    printf("✓ Heap buffer allocated at: %p\n", (void*)sensitive_key);
    
    // Store sensitive key (in real scenario, this would come from secure input)
    strcpy(sensitive_key, "MySecretKey123!@#");
    
    printf("Key stored (length: %zu)\n", strlen(sensitive_key));
    
    // Use the key (simulated)
    printf("\nUsing the key for operations...\n");
    printf("Key value: \"%s\"\n", sensitive_key);
    
    // Perform some operation with the key
    for (int i = 0; i < 5; i++) {
        printf("  Operation %d: using key...\n", i + 1);
    }
    
    // Securely clear the key from memory
    printf("\nSecurely clearing key from memory...\n");
    secure_zero(sensitive_key, key_size);
    
    // Verify it's cleared
    printf("Key after clearing: \"%s\"\n", sensitive_key);
    printf("(Should be empty/null bytes)\n");
    
    // Free the memory
    free(sensitive_key);
    printf("\n✓ Memory freed\n");
    
    return 0;
}