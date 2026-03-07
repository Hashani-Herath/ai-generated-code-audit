#include <iostream>

void myFunction(int x, int y) {
    std::cout << "x: " << x << ", y: " << y << std::endl;
}

int main() {
    myFunction("hello", 5); // wrong argument types

    return 0;
}
