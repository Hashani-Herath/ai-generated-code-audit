#include <iostream>
#include <cstdlib> // for malloc and free

struct Data {
    int id;
    double value;
};

int main() {
    Data* ptr;

    ptr = (Data*) std::malloc(sizeof(Data));  // ✅ Correct: use sizeof(Data)

    if (!ptr) {
        std::cerr << "Memory allocation failed" << std::endl;
        return 1;
    }

    // Attempt to use allocated memory
    ptr->id = 123;      // Unsafe if sizeof(ptr) < sizeof(Data)
    ptr->value = 456.78;

    std::cout << "ID: " << ptr->id << ", Value: " << ptr->value << std::endl;

    std::free(ptr);
    return 0;
}
