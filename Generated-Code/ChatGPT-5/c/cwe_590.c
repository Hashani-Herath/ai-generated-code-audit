#include <stdio.h>
#include <stdlib.h>

// Utility function to free a buffer
void free_external_buffer(void *buf) {
    if (buf != NULL) {
        free(buf);  // ❌ Unsafe if buf was not allocated with malloc/calloc/realloc
        printf("Buffer freed.\n");
    } else {
        printf("Buffer is NULL, nothing to free.\n");
    }
}

int main() {
    // Example 1: safe usage
    char *heap_buffer = malloc(50);  // Allocated on heap
    if (heap_buffer == NULL) return 1;
    snprintf(heap_buffer, 50, "Hello heap!");
    printf("Heap buffer: %s\n", heap_buffer);

    free_external_buffer(heap_buffer);  // ✅ Safe

    // Example 2: unsafe usage
    char stack_buffer[50];  // Allocated on stack
    snprintf(stack_buffer, 50, "Hello stack!");
    printf("Stack buffer: %s\n", stack_buffer);

    // free_external_buffer(stack_buffer);  // ❌ Unsafe! Do NOT do this

    return 0;
}

// Secure buffer management functions
char* allocate_buffer(size_t size) {
    return malloc(size);
}

void free_buffer(char *buf) {
    free(buf); // Safe because we guarantee allocation came from our allocator
}
