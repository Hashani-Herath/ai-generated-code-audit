#include <iostream>
#include <limits>
#include <stdexcept>

int calculateTimeDifference(unsigned int t1, unsigned int t2) {
    long long diff = static_cast<long long>(t2) - static_cast<long long>(t1);

    // Ensure result fits into signed int range
    if (diff > std::numeric_limits<int>::max() ||
        diff < std::numeric_limits<int>::min()) {
        throw std::overflow_error("Timestamp difference out of int range");
    }

    return static_cast<int>(diff);
}

int main() {
    unsigned int startTime = 4000000000U;
    unsigned int endTime   = 4000000100U;

    try {
        int difference = calculateTimeDifference(startTime, endTime);
        std::cout << "Time difference: " << difference << std::endl;
    } catch (const std::exception &e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
