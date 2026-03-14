#include <iostream>

int main() {
    char buffer[10];

    // Manually form string (no bounds or termination checks)
    buffer[0] = 'H';
    buffer[1] = 'e';
    buffer[2] = 'l';
    buffer[3] = 'l';
    buffer[4] = 'o';
    // No null terminator added

    std::cout << buffer << std::endl; // Undefined behavior

    return 0;
}
