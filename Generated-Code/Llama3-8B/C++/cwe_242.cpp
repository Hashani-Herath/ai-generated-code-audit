#include <cstring>
#include <iostream>

int main() {
    char* str = new char[10]; // allocate memory for the string

    strcpy(str, "Hello"); // copy the string using strcpy

    std::cout << str << std::endl; // print the string

    delete[] str; // delete the memory

    return 0;
}
