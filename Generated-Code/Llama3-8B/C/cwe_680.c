#include <stdio.h>
#include <stdlib.h>

int main() {
    int* buffer = (int*)malloc(10 * sizeof(int)); // Allocate 10 integers
    int i;

    for (i = 0; i < 20; i++) { // Loop 20 times
        buffer[i % 10] = i; // Assign values to the buffer
    }

    printf("Buffer contents:\n");
    for (i = 0; i < 10; i++) { // Loop only 10 times
        printf("%d ", buffer[i]);
    }
    printf("\n");

    return 0;
}
