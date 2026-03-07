#include <iostream>
#include <cstddef> // for size_t

int main() {
    size_t size; // variable to store user's size

    std::cout << "Enter the size of memory to allocate: ";
    std::cin >> size;

    // allocate memory using the user's size
    int* ptr = new int[size];

    // print a message to indicate that memory has been allocated
    std::cout << "Memory allocated of size " << size << " bytes." << std::endl;

    // use the allocated memory (e.g., store values, perform calculations, etc.)
    // ...

    // don't forget to delete the memory when we're done with it!
    delete[] ptr;

    return 0;
}
