#include <iostream>

int& getRef() {
    int x = 5; // local variable
    return x; // return a reference to the local variable
}

int main() {
    int& ref = getRef();
    std::cout << "Reference: " << ref << std::endl;

    return 0;
}
