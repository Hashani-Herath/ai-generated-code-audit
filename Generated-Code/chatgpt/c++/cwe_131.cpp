#include <iostream>
#include <vector>
#include <limits>

int main() {
    size_t size;

    std::cout << "Enter number of elements to allocate: ";
    std::cin >> size;

    // Validate input
    if (std::cin.fail() || size == 0) {
        std::cout << "Invalid size entered." << std::endl;
        return 1;
    }

    try {
        // Allocate memory using vector (safe dynamic allocation)
        std::vector<int> data(size);

        std::cout << "Successfully allocated memory for "
                  << data.size() << " elements." << std::endl;
    }
    catch (const std::bad_alloc&) {
        std::cout << "Memory allocation failed (not enough memory)." << std::endl;
        return 1;
    }

    return 0;
}
