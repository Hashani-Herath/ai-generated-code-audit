#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Safe: Return dynamically allocated memory
int* safe_return_array(int size) {
    int *arr = (int*)malloc(size * sizeof(int));
    if (!arr) return NULL;
    
    for (int i = 0; i < size; i++) {
        arr[i] = i * 10;
    }
    
    return arr;  // Safe - heap memory persists
}

// Safe: Use static storage (but be careful with reentrancy)
char* safe_return_static() {
    static char buffer[100];  // Static - exists for program lifetime
    strcpy(buffer, "This is static storage");
    return buffer;
}

// Safe: Caller provides buffer
void safe_fill_buffer(char *buffer, size_t size) {
    strncpy(buffer, "Caller provided buffer", size - 1);
    buffer[size - 1] = '\0';
}

int main() {
    int *heap_array;
    char *static_str;
    char local_buffer[100];
    
    printf("Safe Alternatives\n");
    printf("=================\n\n");
    
    // Heap allocation
    heap_array = safe_return_array(5);
    if (heap_array) {
        printf("Heap array: ");
        for (int i = 0; i < 5; i++) {
            printf("%d ", heap_array[i]);
        }
        printf("\n");
        free(heap_array);
    }
    
    // Static storage
    static_str = safe_return_static();
    printf("Static string: %s\n", static_str);
    
    // Caller-provided buffer
    safe_fill_buffer(local_buffer, sizeof(local_buffer));
    printf("Local buffer: %s\n", local_buffer);
    
    return 0;
}