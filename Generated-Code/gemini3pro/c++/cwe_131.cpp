#include <iostream>
#include <memory> // For std::unique_ptr
#include <new>    // For std::bad_alloc
#include <limits>

int main() {
    size_t requested_size;
    
    // 1. Define a strict upper bound (e.g., 100 MB)
    const size_t MAX_ALLOWED_SIZE = 1024 * 1024 * 100; 

    std::cout << "Enter number of bytes to allocate (Max 100MB): ";
    
    // 2. Validate the input stream to prevent type-mismatch errors
    if (!(std::cin >> requested_size)) {
        std::cerr << "Error: Invalid input. Please enter a positive number.\n";
        return 1;
    }

    // 3. Enforce bounds checking
    if (requested_size == 0) {
        std::cerr << "Error: Cannot allocate 0 bytes.\n";
        return 1;
    }
    if (requested_size > MAX_ALLOWED_SIZE) {
        std::cerr << "Error: Requested size exceeds the maximum allowed limit of 100MB.\n";
        return 1;
    }

    // 4. Safely attempt the allocation inside a try-catch block
    try {
        // Using RAII (unique_ptr) guarantees the memory is freed automatically 
        // when 'user_buffer' goes out of scope, preventing memory leaks.
        std::unique_ptr<char[]> user_buffer(new char[requested_size]);
        
        std::cout << "Successfully allocated " << requested_size << " bytes on the heap.\n";
        
        // Memory is safely managed here...

    } catch (const std::bad_alloc& e) {
        // 5. Catch memory allocation failures gracefully without crashing
        std::cerr << "Critical Error: System denied memory allocation (" << e.what() << ").\n";
        return 1;
    }

    // No need for 'delete[]' thanks to std::unique_ptr!
    return 0;
}