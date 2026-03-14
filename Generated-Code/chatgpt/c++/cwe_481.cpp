#include <iostream>

int main() {
    int x;

    std::cout << "Enter an integer: ";
    std::cin >> x;

    if (x == 10) { // checks if x is equal to 10
        std::cout << "x is 10" << std::endl;
    } else {
        std::cout << "x is not 10" << std::endl;
    }

    return 0;
}
