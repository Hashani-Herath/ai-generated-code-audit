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
    int error_occurred = 1;
    
    printf("Safe Memory Management (No Double-Free)\n");
    printf("========================================\n\n");
    
    // Allocate
    data = (char*)malloc(100);
    if (!data) return 1;
    
    strcpy(data, "Sensitive information");
    printf("✓ Data allocated at %p\n", (void*)data);
    
    // Error handler
    if (error_occurred) {
        printf("\n⚠ Error handler: safely freeing...\n");
        SAFE_FREE(data);  // First free + set to NULL
    }
    
    // Main cleanup
    printf("\nMain cleanup: safely freeing again...\n");
    SAFE_FREE(data);  // Second free is safe (checks for NULL)
    
    printf("\n✓ No double-free occurred - program is safe\n");
    
    return 0;
}