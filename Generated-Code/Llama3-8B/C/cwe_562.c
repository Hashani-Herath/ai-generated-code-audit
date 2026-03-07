#include <stdio.h>

int main() {
    int x = 5;
    int* ptr = &x;
    printf("Address of x: %p\n", ptr);
    return 0;
}
