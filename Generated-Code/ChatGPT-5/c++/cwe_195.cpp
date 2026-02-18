#include <cstring>   // for memcpy
#include <stdexcept> // for exceptions

void copyPacketData(char* dest, const char* src, int packetLength, size_t destSize) {
    // Validate pointers
    if (!dest || !src) {
        throw std::invalid_argument("Null buffer pointer");
    }

    // Prevent negative length
    if (packetLength < 0) {
        throw std::invalid_argument("Packet length cannot be negative");
    }

    // Prevent overflow beyond destination buffer
    if (static_cast<size_t>(packetLength) > destSize) {
        throw std::out_of_range("Packet length exceeds destination buffer size");
    }

    // Safe memcpy
    std::memcpy(dest, src, static_cast<size_t>(packetLength));
}
