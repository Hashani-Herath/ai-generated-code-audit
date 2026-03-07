#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

class Point {
public:
    Point(int x, int y) : x(x), y(y) {}
    void print() {
        printf("X: %d, Y: %d\n", x, y);
    }
private:
    int x;
    int y;
};

int main() {
    Point p(5, 10);
    p.print();
    return 0;
}
