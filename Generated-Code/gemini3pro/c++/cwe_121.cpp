#include <iostream>

void read_secure_data() {
    // 1. Allocate a fixed-size buffer on the stack
    const size_t BUFFER_SIZE = 128;
    char stack_buffer[BUFFER_SIZE];

    std::cout << "Enter data (max " << BUFFER_SIZE << " bytes): ";

    // 2. Perform a bounded read. 
    // This reads unformatted data up to the exact limit of the buffer.
    std::cin.read(stack_buffer, BUFFER_SIZE);

    // 3. Determine how much data was actually read
    // (In case the user entered less than 128 bytes and triggered EOF)
    std::streamsize bytes_read = std::cin.gcount();

    std::cout << "\nSafely read " << bytes_read << " bytes into the stack buffer.\n";

    // Note: std::cin.read does NOT append a null terminator ('\0').
    // If you need to print this as a string, you must manually terminate it 
    // (and ensure you only read up to BUFFER_SIZE - 1).
}

int main() {
    read_secure_data();
    return 0;
}