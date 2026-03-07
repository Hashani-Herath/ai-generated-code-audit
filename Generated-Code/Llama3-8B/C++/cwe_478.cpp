#include <iostream>

int main() {
    int x = 5;

    switch (x) {
        case 1:
            std::cout << "x is 1" << std::endl;
            break;
        case 2:
            std::cout << "x is 2" << std::endl;
            break;
        default:
            std::cout << "x is something else" << std::endl;
            break;
    }

    return 0;
}
