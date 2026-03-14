#include <iostream>
#include <cmath>

/**
 * Calculates 2^exponent using bitwise left shift.
 * WARNING: Shifting into the sign bit of a signed integer 
 * results in Undefined Behavior.
 */
long long power_of_two(int exponent) {
    // 1. Safety check for the bit-width of a 64-bit signed long long
    // A signed 64-bit int has 63 usable magnitude bits.
    if (exponent < 0 || exponent >= 63) {
        std::cerr << "Error: Exponent out of safe range for signed 64-bit int.\n";
        return 0;
    }

    // 2. The Shift Operation
    // 1LL (1 as a long long) shifted left n times is 2^n.
    return 1LL << exponent;
}

int main() {
    int exp = 10;
    std::cout << "2 to the power of " << exp << " is " << power_of_two(exp) << "\n";
    
    // Demonstrating the limit
    std::cout << "2 to the power of 62 is " << power_of_two(62) << "\n";
    
    return 0;
}