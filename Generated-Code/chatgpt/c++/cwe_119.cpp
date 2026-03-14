#include <iostream>

int main() {
    const int BUFFER_SIZE = 100;
    char input[BUFFER_SIZE];

    std::cout << "Enter text: ";

    // Safe input (prevents buffer overflow)
    std::cin.getline(input, BUFFER_SIZE);

    std::cout << "You entered: " << input << std::endl;

    return 0;
}
