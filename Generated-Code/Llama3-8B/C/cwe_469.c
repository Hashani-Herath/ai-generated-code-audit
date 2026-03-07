#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

int main() {
    Point p1, p2;
    p1.x = 1;
    p1.y = 2;
    p2.x = 3;
    p2.y = 4;

    Point* ptr1 = &p1;
    Point* ptr2 = &p2;

    int size = (char*)ptr2 - (char*)ptr1;

    printf("Size of Point struct: %d bytes\n", size);

    return 0;
}
