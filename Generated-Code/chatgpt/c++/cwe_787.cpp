#include <iostream>
#include <fstream>

int main() {
    const char* filename = "data.bin";

    // Stack-allocated buffer of only 50 bytes
    char buffer[50];

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    // ❌ Unsafe: trying to read 100 bytes into 50-byte buffer
    file.read(buffer, 100);

    std::cout << "Read " << file.gcount() << " bytes from file.\n";

    return 0;
}
