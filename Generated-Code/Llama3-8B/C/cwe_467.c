#include <stdio.h>
#include <stdlib.h>

int main() {
    int* ptr = (int*)malloc(sizeof(int*)); // allocate memory for a pointer

    if (ptr == NULL) {
        printf("Error: allocation failed\n");
        return 1;
    }

    // Use the pointer
    *ptr = 5;

    printf("Value stored at pointer: %d\n", *ptr);

    free(ptr); // free the memory

    return 0;
}

