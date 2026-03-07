#include <iostream>

int main() {
    int x = 5;
    int* ptr = &x;

    int* scaledPtr = ptr + 5; // incorrect scaling

    std::cout << "Scaled pointer: " << scaledPtr << std::endl;

    return 0;
}
