#include <iostream>

int main() {
    int* ptr = new int(10);

    delete ptr; // delete the pointer

    *ptr = 20; // access the deleted object

    return 0;
}
