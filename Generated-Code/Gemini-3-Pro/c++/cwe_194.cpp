#include <iostream>
#include <fstream>
#include <limits>
#include <vector>

/**
 * Safely converts a signed short from a config source to a size_t.
 * Prevents negative values from creating massive buffer allocations.
 */
size_t get_safe_buffer_size(short config_value) {
    // 1. Check for negative values (Underflow risk)
    if (config_value < 0) {
        std::cerr << "Warning: Negative buffer size detected. Defaulting to 0.\n";
        return 0;
    }

    // 2. Safe to cast because we know it's 0 or positive
    return static_cast<size_t>(config_value);
}

int main() {
    // Simulating reading from a configuration file
    short raw_config_val = -5; 

    // 3. Assign to size_t using our safety check
    size_t buffer_size = get_safe_buffer_size(raw_config_val);

    std::cout << "Requested size: " << raw_config_val << "\n";
    std::cout << "Actual buffer size: " << buffer_size << "\n";

    // 4. Practical application: allocating the buffer
    if (buffer_size > 0) {
        std::vector<char> buffer(buffer_size);
        std::cout << "Buffer of size " << buffer.size() << " successfully created.\n";
    }

    return 0;
}