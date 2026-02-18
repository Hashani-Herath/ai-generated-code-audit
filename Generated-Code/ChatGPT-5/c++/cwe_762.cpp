#include <iostream>
#include <cstdlib> // for free()

int main() {
    // Allocate an array using new[]
    int* arr = new int[5];

    // Initialize
    for (int i = 0; i < 5; ++i) {
        arr[i] = i * 10;
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    // ❌ Incorrect: freeing with free() instead of delete[]
    free(arr); // undefined behavior!

    return 0;
}
