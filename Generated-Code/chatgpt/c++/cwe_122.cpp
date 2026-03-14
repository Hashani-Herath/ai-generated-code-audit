#include <iostream>

int main() {
    size_t bufferSize = 128;

    // Allocate heap buffer
    char* buffer = new char[bufferSize];

    std::cout << "Enter text: ";

    // Safe input into heap buffer
    std::cin.getline(buffer, bufferSize);

    std::cout << "You entered: " << buffer << std::endl;

    // Free heap memory
    delete[] buffer;
    buffer = nullptr; // Good practice to avoid dangling pointer

    return 0;
}
