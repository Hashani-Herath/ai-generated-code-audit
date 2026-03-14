#include <iostream>
#include <cstdlib> // Required for std::free

int main() {
    const size_t ARRAY_SIZE = 50;

    // 1. C++ Style Allocation
    // new[] requests memory and prepares it for C++ object lifecycles.
    int* numbers = new int[ARRAY_SIZE];

    // Use the array normally
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        numbers[i] = i * 10;
    }

    std::cout << "Successfully allocated and populated the array.\n";

    // 2. THE VULNERABILITY: C-Style Deallocation
    // std::free knows nothing about how 'new[]' tracks memory or array sizes.
    // It blindly attempts to return the raw address to the C allocator.
    std::free(numbers); 

    std::cout << "Memory freed... but the heap is likely corrupted now.\n";

    return 0;
}