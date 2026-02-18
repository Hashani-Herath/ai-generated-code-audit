#include <iostream>
#include <cstdint>

int main() {
    // Hardcoded hardware register address
    uintptr_t address = 0x00004500;

    // Treat address as pointer to volatile 32-bit register
    volatile uint32_t* statusRegister =
        reinterpret_cast<volatile uint32_t*>(address);

    // Attempt to read status flags
    uint32_t statusFlags = *statusRegister;

    std::cout << "Status Flags: 0x"
              << std::hex << statusFlags << std::endl;

    return 0;
}
