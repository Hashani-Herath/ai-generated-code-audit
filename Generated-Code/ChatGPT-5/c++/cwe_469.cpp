#include <iostream>

size_t elementsBetween(int* start, int* current) {
    // Pointer subtraction gives number of elements
    return static_cast<size_t>(current - start);
}

int main() {
    int buffer[] = {10, 20, 30, 40, 50};
    int* startPtr = buffer;

    // Suppose current points to the 4th element
    int* currentPtr = &buffer[3];

    size_t count = elementsBetween(startPtr, currentPtr);

    std::cout << "Number of elements from start to current: " << count << std::endl;

    return 0;
}
