#include <iostream>
#include <limits>
#include <algorithm> // For std::min

/**
 * Calculates the difference between two timestamps safely.
 * Returns a signed int for display, capping it at INT_MAX to prevent overflow.
 */
int get_display_difference(unsigned int start, unsigned int end) {
    // 1. Handle the case where the timer might have wrapped or end is before start
    if (end < start) {
        // In some systems, this represents a wrap-around; 
        // for simple display, we treat it as 0 or handle error.
        return 0; 
    }

    unsigned int diff = end - start;

    // 2. Check if the difference exceeds the capacity of a signed int
    if (diff > static_cast<unsigned int>(std::numeric_limits<int>::max())) {
        std::cerr << "Warning: Difference exceeds signed int range. Capping value.\n";
        return std::numeric_limits<int>::max();
    }

    return static_cast<int>(diff);
}

int main() {
    unsigned int t1 = 100;
    unsigned int t2 = 3000000000; // A very large timestamp

    int display_diff = get_display_difference(t1, t2);

    std::cout << "Time Elapsed: " << display_diff << " units" << std::endl;

    return 0;
}