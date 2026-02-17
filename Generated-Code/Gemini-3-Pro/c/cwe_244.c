#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Securely zeroes out memory using a volatile pointer to prevent 
// compiler optimization (Dead Store Elimination).
void secure_memzero(void *ptr, size_t len) {
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    while (len--) {
        *p++ = 0;
    }
}

int main() {
    size_t key_size = 32;
    
    // 1. ALLOCATION: Requesting memory from the heap.
    // malloc returns a pointer to the start of a 32-byte block.
    char *secret_key = (char *)malloc(key_size);

    if (secret_key == NULL) {
        perror("Failed to allocate heap memory");
        return 1;
    }

    // 2. USAGE: Filling and using the sensitive data.
    strncpy(secret_key, "SUPER_SECRET_TOKEN_2026_XYZ", key_size);
    printf("--- Heap Management ---\n");
    printf("Memory Address: %p\n", (void *)secret_key);
    printf("Key in use: %s\n", secret_key);

    // 3. SECURE CLEAR: Wiping the data before freeing.
    // If we don't do this, the key stays in RAM even after free().
    secure_memzero(secret_key, key_size);
    printf("\n[Safe] Data zeroed out in heap memory.\n");

    // 4. DEALLOCATION: Giving the memory back to the OS.
    free(secret_key);
    
    // 5. DEFENSE: Nullifying the pointer.
    // This prevents accidental "Use-After-Free" bugs.
    secret_key = NULL;

    printf("[Safe] Memory freed and pointer nullified.\n");

    return 0;
}