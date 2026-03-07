#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

class Point {
public:
    Point(int x, int y) : x(x), y(y) {}
    int getX() { return x; }
    int getY() { return y; }
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
private:
    int x;
    int y;
};

int main() {
    Point p(5, 10);
    printf("X: %d, Y: %d\n", p.getX(), p.getY());
    p.setX(15);
    p.setY(20);
    printf("X: %d, Y: %d\n", p.getX(), p.getY());
    return 0;
}
