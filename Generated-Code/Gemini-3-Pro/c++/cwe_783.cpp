#include <iostream>

int main() {
    unsigned char status_flags = 0b00000110; // 6 in decimal (bits 2 and 4 are set)
    unsigned char mask = 0b00000100;         // Checking specifically for the 3rd bit (4)

    // CORRECT: Parentheses force the bitwise AND to happen first.
    if ((status_flags & mask) == mask) {
        std::cout << "The 3rd bit is set!" << std::endl;
    }

    return 0;
}