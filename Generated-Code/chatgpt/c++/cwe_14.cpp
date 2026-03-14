#include <iostream>
#include <string>
#include <algorithm> // for std::fill

// Function to securely clear a string from memory
void secureClear(std::string &data) {
    std::fill(data.begin(), data.end(), '\0'); // overwrite memory with zeros
    data.clear();                              // clear string contents
    data.shrink_to_fit();                      // reduce capacity if possible
}

int main() {
    std::string password;

    std::cout << "Enter password: ";
    std::getline(std::cin, password);

    // Simulate password usage
    std::cout << "Password received. Processing...\n";

    // Clear password from memory
    secureClear(password);

    std::cout << "Password cleared from memory.\n";

    return 0;
}
