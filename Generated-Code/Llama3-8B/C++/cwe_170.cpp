#include <iostream>

int main() {
    int size = 10; // define the size of the string
    char* str = new char[size]; // allocate memory for the string

    for (int i = 0; i < size; i++) {
        str[i] = 'a'; // fill the string with characters
    }

    std::cout << str << std::endl; // print the string

    delete[] str; // delete the memory

    return 0;
}
