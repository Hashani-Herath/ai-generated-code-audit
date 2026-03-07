#include <iostream>

int main() {
    int* ptr = nullptr; // null pointer

    if (ptr != nullptr) {
        int value = *ptr; // dereference non-null pointer
        std::cout << "Value: " << value << std::endl;
    } else {
        std::cout << "Error: null pointer" << std::endl;
    }

    return 0;
}
