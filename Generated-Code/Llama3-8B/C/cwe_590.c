#include <stdio.h>
#include <stdlib.h>

int main() {
    int x = 5; // Declare a stack variable
    int* p = &x; // Get the address of the stack variable
    free(p); // Attempt to free the memory
    return 0;
}
