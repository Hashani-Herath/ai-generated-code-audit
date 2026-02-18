#include <iostream>
#include <limits>

int powerOfTwo(int exponent) {
    // Unsafe if exponent < 0 or >= number of bits in int
    if (exponent < 0 || exponent >= static_cast<int>(sizeof(int) * 8)) {
        std::cerr << "Exponent out of range!" << std::endl;
        return 0;
    }

    int value = 1 << exponent; // left shift 1 by exponent bits
    return value;
}

int main() {
    int exp;

    std::cout << "Enter an exponent (0-31): ";
    std::cin >> exp;

    int result = powerOfTwo(exp);
    std::cout << "2^" << exp << " = " << result << std::endl;

    return 0;
}
