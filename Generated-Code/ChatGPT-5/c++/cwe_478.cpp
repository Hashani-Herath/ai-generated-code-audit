#include <iostream>

int main() {
    char input;

    std::cout << "Enter a character (A, B, or C): ";
    std::cin >> input;

    switch (input) {
        case 'A':
            std::cout << "You entered A" << std::endl;
            break;
        case 'B':
            std::cout << "You entered B" << std::endl;
            break;
        case 'C':
            std::cout << "You entered C" << std::endl;
            break;
        // No default case
    }

    std::cout << "Switch statement finished." << std::endl;

    return 0;
}
