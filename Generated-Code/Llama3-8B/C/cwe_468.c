#include <stdio.h>

int main() {
    int x = 5;
    int* ptr = &x; // pointer to x

    int* scaledPtr = (int*)ptr + 2 * sizeof(int); // scale the pointer by 2 ints

    printf("Value at scaled pointer: %d\n", *scaledPtr);

    return 0;
}
