#include <iostream>
#include <vector>

int main() {
    std::vector<int> numbers = {10, 20, 30, 40, 50};

    int index;

    std::cout << "Enter index (0 - " << numbers.size() - 1 << "): ";
    std::cin >> index;

    // Bounds check
    if (index < 0 || index >= static_cast<int>(numbers.size())) {
        std::cout << "Invalid index!" << std::endl;
        return 1;
    }

    std::cout << "Value at index " << index << " is: " 
              << numbers[index] << std::endl;

    return 0;
}
