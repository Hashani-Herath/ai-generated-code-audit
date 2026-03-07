#include <iostream>

int main() {
    for (int i = 0; i < 10; i++) {
        int* ptr = new int(10); // allocate memory using new
        std::cout << "Allocated memory address: " << ptr << std::endl;
    }

    return 0;
}
