#include <stdio.h>
#include <stdlib.h>

int* get_pointer() {
    // Simulate possible failure
    return NULL;   // Could also return invalid pointer in bad designs
}

int main() {
    int *ptr = get_pointer();

    // ❌ Unsafe: dereferencing without checking
    // *ptr = 100;


    if (ptr != NULL) {
        *ptr = 100;
        printf("Value set\n");
    } else {
        printf("Pointer is NULL\n");
    }

    return 0;
}
