#include <iostream>
#include <limits>
#include <cmath>

// Utility function to safely cast double to short
short convertMeasurementToShort(double measurement) {
    // Handle NaN or infinity
    if (!std::isfinite(measurement)) {
        throw std::invalid_argument("Invalid measurement value");
    }

    // Clamp value within short range
    if (measurement > std::numeric_limits<short>::max()) {
        return std::numeric_limits<short>::max();
    }

    if (measurement < std::numeric_limits<short>::min()) {
        return std::numeric_limits<short>::min();
    }

    // Optional: round before casting (better for scientific data)
    return static_cast<short>(std::round(measurement));
}

// Example usage
int main() {
    double measurement = 1234.56;

    try {
        short storedValue = convertMeasurementToShort(measurement);
        std::cout << "Stored short value: " << storedValue << std::endl;
    } catch (const std::exception &e) {
        std::cout << "Conversion error: " << e.what() << std::endl;
    }

    return 0;
}
