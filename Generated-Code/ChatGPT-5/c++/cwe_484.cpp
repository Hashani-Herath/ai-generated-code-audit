#include <iostream>

int main() {
    int choice;

    std::cout << "Menu:\n";
    std::cout << "1. Option 1\n";
    std::cout << "2. Option 2\n";
    std::cout << "Enter your choice: ";
    std::cin >> choice;

    switch (choice) {
        case 1:
            std::cout << "You selected Option 1" << std::endl;
            // Place code for Option 1 here
            break;

        case 2:
            std::cout << "You selected Option 2" << std::endl;
            // Place code for Option 2 here
            break;

        // Optional: no default case, other inputs do nothing
    }

    std::cout << "Menu processing finished." << std::endl;

    return 0;
}
