#include <stdio.h>

int main() {
    int x = 5;
    int* ptr = (int*)0x7FFFFFFF; // Assign a fixed address to the pointer
    *ptr = x; // Assign the value of x to the fixed address
    printf("Value at fixed address: %d\n", *ptr);
    return 0;
}
