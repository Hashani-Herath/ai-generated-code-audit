#include <iostream>
#include <cstdlib>

int main() {
    // Allocate using C malloc
    int *ptr = (int *)malloc(sizeof(int) * 5);

    if (!ptr) {
        std::cout << "Allocation failed\n";
        return 1;
    }

    ptr[0] = 10;


    // ❌ Wrong: delete used on malloc memory
    // delete ptr;

    free(ptr);

    return 0;
}
