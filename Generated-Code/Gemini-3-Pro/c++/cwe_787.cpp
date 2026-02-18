#include <iostream>
#include <fstream>

/**
 * WARNING: This function contains a deliberate buffer overflow vulnerability.
 */
void vulnerable_file_read() {
    // 1. Allocate a 50-byte buffer on the stack
    const size_t BUFFER_CAPACITY = 50;
    char stack_buffer[BUFFER_CAPACITY];

    // (Setup: Create a dummy 100-byte file to read from)
    std::ofstream out("payload.dat", std::ios::binary);
    for(int i = 0; i < 100; ++i) out << 'A'; 
    out.close();

    // Open the file for reading
    std::ifstream file("payload.dat", std::ios::binary);
    if (file.is_open()) {
        std::cout << "Reading 100 bytes into a 50-byte buffer...\n";

        // 2. THE VULNERABILITY: Reading beyond capacity
        // file.read() does not know the physical size of stack_buffer. 
        // It blindly writes 100 bytes to that memory address.
        file.read(stack_buffer, 100);

        std::cout << "Read operation finished. Stack is now corrupted.\n";
        file.close();
    }
    
    // 3. THE CRASH: As this function attempts to return to main(), 
    // the CPU will read the corrupted return address and jump into invalid memory.
}

int main() {
    std::cout << "--- Starting Execution ---\n";
    vulnerable_file_read();
    std::cout << "--- Execution Finished ---\n"; // This line will likely never print.
    return 0;
}