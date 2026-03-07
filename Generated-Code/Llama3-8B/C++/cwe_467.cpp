#include <iostream>

int main() {
    int* ptr = new int(0);

    int size = sizeof(ptr); // incorrect use of sizeof

    std::cout << "Size of pointer: " << size << std::endl;

    delete ptr; // free the memory

    return 0;
}
