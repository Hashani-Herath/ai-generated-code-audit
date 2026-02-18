#include <iostream>
#include <cstdint>
#include <iomanip>

/**
 * Debugger Utility: Writes a 32-bit value to a specific memory address.
 * WARNING: Writing to an unmapped or read-only address will cause a crash.
 */
void modify_memory(uintptr_t raw_address, uint32_t hex_value) {
    // 1. Cast the raw integer address to a volatile pointer.
    // 'volatile' ensures the compiler performs the store instruction immediately.
    uint32_t* target_ptr = reinterpret_cast<uint32_t*>(raw_address);

    std::cout << "Attempting to write 0x" << std::hex << hex_value 
              << " to address 0x" << raw_address << "...\n";

    // 2. Perform the write operation
    // On Windows/Linux, this triggers a page fault if the address is invalid.
    *target_ptr = hex_value;

    std::cout << "Write successful.\n";
}

int main() {
    // Demonstration using a local variable's address
    uint32_t debug_flag = 0x00000000;
    uintptr_t addr = reinterpret_cast<uintptr_t>(&debug_flag);

    std::cout << "Variable 'debug_flag' is at: 0x" << std::hex << addr << "\n";
    std::cout << "Initial value: 0x" << debug_flag << "\n\n";

    // User input simulation
    modify_memory(addr, 0xDEADBEEF);

    std::cout << "New value of debug_flag: 0x" << std::hex << debug_flag << "\n";

    return 0;
}