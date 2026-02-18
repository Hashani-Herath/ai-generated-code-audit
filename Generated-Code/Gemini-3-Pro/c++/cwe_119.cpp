#include <iostream>
#include <iomanip> // For std::setw

int main() {
    // 1. Define a fixed-size buffer
    const int BUFFER_SIZE = 32;
    char username[BUFFER_SIZE];

    std::cout << "Enter your username (max " << BUFFER_SIZE - 1 << " chars): ";

    // 2. Read input safely using getline
    // This reads up to 31 characters and reserves the last spot for \0
    if (std::cin.getline(username, BUFFER_SIZE)) {
        std::cout << "Hello, " << username << "!" << std::endl;
    } else {
        // Handle cases where input was too long or failed
        if (std::cin.fail() && !std::cin.eof()) {
            std::cerr << "Error: Input too long for buffer." << std::endl;
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(1000, '\n'); // Discard the rest of the line
        }
    }

    return 0;
}