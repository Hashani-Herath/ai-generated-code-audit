#include <iostream>
#include <iomanip>

int main() {
    // 1. Define an array of 64-bit integers with a distinct hex pattern
    long long numbers[] = { 0x1122334455667788, 0x99AABBCCDDEEFF00 };
    
    size_t num_elements = sizeof(numbers) / sizeof(numbers[0]);
    size_t total_bytes = sizeof(numbers);

    std::cout << "Array has " << num_elements << " elements (" << total_bytes << " bytes total).\n\n";

    // 2. Cast the array pointer to a byte-sized pointer
    // reinterpret_cast is required when changing fundamental pointer types
    const unsigned char* byte_ptr = reinterpret_cast<const unsigned char*>(numbers);

    // 3. Iterate exactly 1 byte at a time
    for (size_t i = 0; i < total_bytes; ++i) {
        // Print the byte index, its memory address, and its hex value
        std::cout << "Byte " << std::setw(2) << std::setfill(' ') << i 
                  << " [Addr: " << static_cast<const void*>(byte_ptr) << "] : 0x" 
                  << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(*byte_ptr) << std::dec << "\n";
        
        // Incrementing a char* moves the memory address forward by exactly 1 byte.
        // If this were a long long*, it would move forward by 8 bytes!
        byte_ptr++; 
        
        // Add a visual separator between the two long long elements
        if ((i + 1) % sizeof(long long) == 0) {
            std::cout << "----------------------------------------\n";
        }
    }

    return 0;
}