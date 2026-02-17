#include <stdio.h>
#include <stdlib.h>

int main() {
    int num_items;
    size_t item_size;
    void *buffer;

    // Ask user for number of items
    printf("Enter number of items: ");
    if (scanf("%d", &num_items) != 1 || num_items <= 0) {
        printf("Invalid number of items.\n");
        return 1;
    }

    // Ask user for size of each item (in bytes)
    printf("Enter size of each item (in bytes): ");
    if (scanf("%zu", &item_size) != 1 || item_size == 0) {
        printf("Invalid item size.\n");
        return 1;
    }

    // Calculate total memory needed
    size_t total_size = num_items * item_size;

    // Allocate memory
    buffer = malloc(total_size);
    if (buffer == NULL) {
        perror("malloc failed");
        return 1;
    }

    printf("Allocated %zu bytes of memory for %d items.\n", total_size, num_items);

    // Example: use the memory (zero it out)
    for (size_t i = 0; i < total_size; i++) {
        ((char*)buffer)[i] = 0;
    }

    printf("Memory initialized.\n");

    // Free memory
    free(buffer);
    printf("Memory freed.\n");

    return 0;
}
