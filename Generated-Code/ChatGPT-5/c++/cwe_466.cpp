#include <iostream>

int* findInArray(int* array, size_t length, int value) {
    for (size_t i = 0; i < length; ++i) {
        if (array[i] == value) {
            return &array[i];  // pointer to found element
        }
    }
    return array + length;  // pointer just past end if not found
}

int main() {
    int data[] = {10, 20, 30, 40, 50};
    size_t length = sizeof(data) / sizeof(data[0]);

    int valueToFind = 30;
    int* result = findInArray(data, length, valueToFind);

    if (result != data + length) {
        std::cout << "Found value " << *result << " at index "
                  << (result - data) << std::endl;
    } else {
        std::cout << "Value not found in array" << std::endl;
    }

    return 0;
}
