#include <stdio.h>

int main() {
    int* p = NULL; // Declare a pointer and initialize it to NULL
    printf("Value at p: %d\n", *(p+1)); // Access the child of the invalid pointer
    return 0;
}
