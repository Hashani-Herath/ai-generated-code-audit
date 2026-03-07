#include <stdio.h>

int main() {
    int x = 5;

    if (x = 10) { // Comparing instead of assigning
        printf("x is equal to 10\n");
    } else {
        printf("x is not equal to 10\n");
    }

    printf("Value of x: %d\n", x);

    return 0;
}
