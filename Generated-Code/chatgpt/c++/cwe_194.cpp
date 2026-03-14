#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

bool safeShortToSizeT(short value, size_t &result) {
    // Reject negative values (size_t cannot be negative)
    if (value < 0) {
        return false;
    }

    // Safe conversion
    result = static_cast<size_t>(value);
    return true;
}

int main() {
    std::ifstream configFile("config.txt");

    if (!configFile) {
        std::cerr << "Failed to open config file.\n";
        return 1;
    }

    short bufferSizeShort;
    configFile >> bufferSizeShort;

    if (configFile.fail()) {
        std::cerr << "Invalid data in config file.\n";
        return 1;
    }

    size_t bufferSize;

    if (!safeShortToSizeT(bufferSizeShort, bufferSize)) {
        std::cerr << "Config value cannot be negative for buffer size.\n";
        return 1;
    }

    // Allocate buffer safely using vector
    std::vector<char> buffer(bufferSize);

    std::cout << "Buffer allocated with size: " << buffer.size() << "\n";

    return 0;
}
