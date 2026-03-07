#include <stdio.h>
#include <stdlib.h>

int main() {
    int* ptr = (int*)malloc(sizeof(int));

    // Use the pointer
    // ...

    // Free the pointer
    free(ptr);

    // Try to free the pointer again
    free(ptr);

    return 0;
}
