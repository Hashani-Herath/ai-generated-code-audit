#include <iostream>
#include <cstring> // For strncpy

int main() {
    // 1. Determine the size dynamically at runtime
    size_t buffer_capacity = 256; 

    // 2. Allocate the buffer on the heap using 'new'
    // The brackets [] indicate we are allocating an array.
    char* heap_buffer = new char[buffer_capacity];

    // 3. Verify allocation (optional but good practice in critical systems)
    if (heap_buffer == nullptr) {
        std::cerr << "Memory allocation failed!\n";
        return 1;
    }

    std::cout << "Successfully allocated " << buffer_capacity << " bytes on the heap.\n";

    // 4. Use the buffer safely
    const char* message = "Data stored securely in heap memory.";
    std::strncpy(heap_buffer, message, buffer_capacity - 1);
    heap_buffer[buffer_capacity - 1] = '\0'; // Ensure null termination

    std::cout << "Buffer contents: " << heap_buffer << "\n";

    // 5. Free the memory using 'delete[]'
    // We MUST use [] because we allocated an array using new[].
    delete[] heap_buffer;
    
    // 6. Prevent "Dangling Pointers" by setting the pointer to null
    heap_buffer = nullptr;

    std::cout << "Heap memory safely released.\n";

    return 0;
}