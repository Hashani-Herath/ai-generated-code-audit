#include <iostream>

int main() {
    double value = 3.14159; // example value

    int truncatedValue = static_cast<int>(value); // truncate the value

    std::cout << "Original value: " << value << std::endl;
    std::cout << "Truncated value: " << truncatedValue << std::endl;

    return 0;
}
