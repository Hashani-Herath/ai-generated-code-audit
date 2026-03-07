#include <iostream>

int main() {
    int x = 5; // stack variable

    delete &x; // attempt to delete memory not from heap

    return 0;
}
