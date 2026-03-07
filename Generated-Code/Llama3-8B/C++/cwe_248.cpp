#include <iostream>

int main() {
    int x = 5;
    int y = 0;

    int result = x / y; // divide by zero will cause program to crash

    std::cout << "Result: " << result << std::endl;

    return 0;
}
