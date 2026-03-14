#include <iostream>
#include <cstdint>

/**
 * Attempts to read a 32-bit status register from a specific memory address.
 * WARNING: This will cause a Segmentation Fault on modern OS user-space applications.
 */
void read_hardware_status() {
    // 1. Define the hardcoded physical address specified by the hardware datasheet
    const uintptr_t STATUS_REGISTER_ADDR = 0x00004500;

    // 2. Cast the raw integer to a pointer of the correct size (e.g., 32-bit).
    // The 'volatile' keyword is mandatory here. It tells the compiler: 
    // "Hardware can change this value at any time. Do NOT optimize or cache this read."
    volatile uint32_t* hardware_ptr = reinterpret_cast<volatile uint32_t*>(STATUS_REGISTER_ADDR);

    std::cout << "Attempting to read from hardware address 0x" << std::hex << STATUS_REGISTER_ADDR << "...\n";

    // 3. Dereference the pointer to trigger the memory bus read
    uint32_t current_flags = *hardware_ptr;

    // 4. Isolate specific bits (e.g., checking if bit 0 is set)
    if (current_flags & 0x00000001) {
        std::cout << "Hardware Status: READY\n";
    } else {
        std::cout << "Hardware Status: BUSY\n";
    }
}

int main() {
    // In an embedded system (bare-metal), this works perfectly.
    // On Windows/Linux/macOS, uncommenting this will instantly crash the program.
    // read_hardware_status(); 

    std::cout << "Driver snippet compiled safely.\n";
    return 0;
}