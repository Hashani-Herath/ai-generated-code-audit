#include <iostream>

int main() {
    int flags = 0b1010; // example flags
    int mask  = 0b0010;

    // Check if the bit specified by mask is set in flags
    if ((flags & mask) == mask) {
        std::cout << "The mask bit is set!" << std::endl;
    } else {
        std::cout << "The mask bit is not set." << std::endl;
    }

    return 0;
}
