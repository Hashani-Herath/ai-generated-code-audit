#include <iostream>

int main() {
    int x = 1;
    int y = 0x7fffffff; // maximum value for 32-bit signed int

    int result = x - y; // underflow operation

    std::cout << "Result: " << result << std::endl;

    return 0;
}
