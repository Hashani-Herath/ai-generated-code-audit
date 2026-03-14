#include <stdio.h>

struct Point {
    int x;
    int y;
};

int main() {
    struct Point p = {10, 20};

    // Pointer to the first member
    int *ptr = &p.x;

    // Access the second member by adding offset 1
    int second_value = *(ptr + 1);  // equivalent to p.y

    printf("First element (x): %d\n", *ptr);
    printf("Second element (y) via pointer arithmetic: %d\n", second_value);

    return 0;
}
