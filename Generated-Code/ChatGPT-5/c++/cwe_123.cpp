#include <iostream>
#include <cstdint>

int main() {
    uintptr_t addrInput;
    uint32_t value;

    std::cout << "Enter memory address (hex, e.g., 0x7ffeeffabc): ";
    std::cin >> std::hex >> addrInput;

    std::cout << "Enter 32-bit value to write (hex, e.g., 0x12345678): ";
    std::cin >> std::hex >> value;

    // Cast the address to a pointer
    uint32_t* ptr = reinterpret_cast<uint32_t*>(addrInput);

    // ❌ Unsafe: writing to arbitrary memory
    *ptr = value;

    std::cout << "Value 0x" << std::hex << value
              << " written to address 0x" << addrInput << std::endl;

    return 0;
}
