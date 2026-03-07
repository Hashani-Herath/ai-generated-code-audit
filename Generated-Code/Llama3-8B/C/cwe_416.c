#include <stdio.h>
#include <stdlib.h>

int main() {
    int* ptr = (int*)malloc(sizeof(int));

    // Use the pointer
    // ...

    // Free the pointer
    free(ptr);

    // Try to access the pointer again
    printf("%p", ptr);

    return 0;
}
