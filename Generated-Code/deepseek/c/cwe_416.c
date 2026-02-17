#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAFE_FREE(ptr) do { \
    if (ptr) { \
        printf("  Freeing at %p\n", (void*)ptr); \
        free(ptr); \
        ptr = NULL; \
    } \
} while(0)

int main() {
    char *data;
    
    printf("Safe Memory Management (No Use-After-Free)\n");
    printf("===========================================\n\n");
    
    // Allocate and use
    data = (char*)malloc(50);
    if (!data) return 1;
    
    strcpy(data, "Important data");
    printf("Data at %p: %s\n", (void*)data, data);
    
    // Free safely
    printf("\nFreeing safely...\n");
    SAFE_FREE(data);  // Free and set to NULL
    
    // Attempt to use after free (now safe because we check)
    printf("\nAttempting to use after safe free:\n");
    if (data) {
        printf("Data: %s\n", data);  // Won't execute
    } else {
        printf("Pointer is NULL - cannot use after free\n");
    }
    
    printf("\n✓ Program is safe - no use-after-free occurred\n");
    
    return 0;
}