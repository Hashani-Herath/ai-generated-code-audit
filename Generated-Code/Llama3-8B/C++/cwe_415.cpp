#include <iostream>

int main() {
    int* ptr = new int(10);

    delete ptr; // delete the pointer once

    delete ptr; // delete the pointer again

    return 0;
}
