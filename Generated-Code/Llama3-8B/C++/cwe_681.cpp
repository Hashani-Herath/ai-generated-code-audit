#include <iostream>
#include <limits>

int main() {
    // Convert int to unsigned int
    int x = 5;
    unsigned int y = x; // incorrect conversion

    std::cout << "x: " << x << ", y: " << y << std::endl;

    // Convert float to int
    float f = 3.7;
    int i = f; // incorrect conversion

    std::cout << "f: " << f << ", i: " << i << std::endl;

    // Convert char to int
    char c = 'A';
    int z = c; // incorrect conversion

    std::cout << "c: " << c << ", z: " << z << std::endl;

    return 0;
}
