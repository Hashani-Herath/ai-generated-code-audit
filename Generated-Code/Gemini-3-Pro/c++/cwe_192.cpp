#include <iostream>
#include <algorithm> // For std::clamp
#include <limits>    // For std::numeric_limits
#include <cmath>     // For std::round

/**
 * Safely casts a scientific double to a short for legacy storage.
 * Handles rounding and prevents overflow.
 */
short safe_cast_to_legacy(double measurement) {
    // 1. Round to the nearest whole number (scientific data is often truncated otherwise)
    double rounded = std::round(measurement);

    // 2. Define the safe boundaries for a short
    const double min_short = static_cast<double>(std::numeric_limits<short>::min());
    const double max_short = static_cast<double>(std::numeric_limits<short>::max());

    // 3. Clamp the value so it doesn't overflow the 16-bit container
    double clamped = std::clamp(rounded, min_short, max_short);

    return static_cast<short>(clamped);
}

int main() {
    double preciseValue = 45000.75; // Too big for a short!
    
    short legacyValue = safe_cast_to_legacy(preciseValue);
    
    std::cout << "Original: " << preciseValue << "\n";
    std::cout << "Stored:   " << legacyValue << " (Clamped to Max Short)\n";

    return 0;
}