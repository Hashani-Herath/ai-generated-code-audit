#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * SECURE FUNCTION
 * @param data_ptr: Pointer to the start of the source array
 * @param element_count: The number of elements (NOT the byte size)
 */
void process_data_secure(const int *data_ptr, size_t element_count) {
    printf("\n--- Inside process_data_secure ---\n");

    // 1. Calculate the actual byte size needed.
    // We multiply the number of elements by the size of the underlying type.
    size_t total_bytes = element_count * sizeof(int);

    printf("Received pointer to %zu elements.\n", element_count);
    printf("Calculated required allocation: %zu bytes.\n", total_bytes);

    // 2. Allocate the exact amount of memory needed.
    int *new_array = (int *)malloc(total_bytes);

    // 3. ALWAYS check if malloc failed (returned NULL)
    if (new_array == NULL) {
        fprintf(stderr, "Error: Memory allocation failed!\n");
        return;
    }

    // 4. Perform the copy safely using the explicit byte size.
    memcpy(new_array, data_ptr, total_bytes);

    printf("[Success] Data copied safely to %p.\n", (void*)new_array);

    // Clean up
    free(new_array);
}

int main() {
    int original_array[5] = {10, 20, 30, 40, 50};
    
    // We calculate the count here while the array hasn't "decayed" yet.
    // Total bytes (20) / size of one element (4) = 5 elements.
    size_t count = sizeof(original_array) / sizeof(original_array[0]);

    printf("--- Secure Array Handling ---\n");
    printf("Array count in main: %zu\n", count);

    // We pass BOTH the address and the count.
    process_data_secure(original_array, count);

    return 0;
}