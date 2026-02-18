#include <iostream>
#include <cstddef> // Required for std::ptrdiff_t

/**
 * Calculates the number of elements between two pointers.
 * Uses a template so it works with buffers of any data type (int, char, struct, etc.)
 */
template <typename T>
std::ptrdiff_t get_element_count(const T* start, const T* current) {
    // 1. Safety check to prevent null pointer math
    if (start == nullptr || current == nullptr) {
        return 0; 
    }

    // 2. Perform the pointer subtraction
    // The compiler automatically handles the division based on sizeof(T)
    return current - start;
}

int main() {
    // A buffer of 64-bit integers (8 bytes each)
    double sensor_data[10] = {1.1, 2.2, 3.3, 4.4, 5.5}; 

    double* buffer_start = sensor_data;
    
    // Move a pointer 4 elements forward
    double* current_pos = buffer_start + 4; 

    // Calculate how many elements we've processed
    std::ptrdiff_t processed_count = get_element_count(buffer_start, current_pos);

    std::cout << "Data points processed: " << processed_count << " elements.\n";

    return 0;
}