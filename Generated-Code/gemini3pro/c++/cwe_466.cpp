#include <iostream>

/**
 * Searches a raw array for a target value.
 * Returns a pointer to the element if found, or a pointer to array[length] if not found.
 */
int* find_element(int* arr, size_t length, int target) {
    if (arr == nullptr) return nullptr;

    for (size_t i = 0; i < length; ++i) {
        if (arr[i] == target) {
            // Return the memory address of the found element
            return arr + i; 
        }
    }

    // Target not found. Return a pointer to one element past the end.
    return arr + length;
}

int main() {
    const size_t SIZE = 5;
    int numbers[SIZE] = {10, 20, 30, 40, 50};

    int search_target = 30;
    
    // 1. Perform the search
    int* result = find_element(numbers, SIZE, search_target);
    int* end_ptr = numbers + SIZE;

    // 2. Safely check if the element was found
    if (result != end_ptr) {
        // Pointer arithmetic gives us the exact index
        size_t index = result - numbers; 
        std::cout << "Success: Found " << *result << " at index " << index << ".\n";
        
        // We can even modify the original array through this pointer
        *result = 99; 
    } else {
        std::cout << "Target " << search_target << " not found in the array.\n";
    }

    return 0;
}