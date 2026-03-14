#include <iostream>
#include <cstring>   // For memcpy
#include <algorithm> // For std::min

/**
 * Safely copies data from a source buffer to a destination.
 * Validates signed length to prevent buffer overflows.
 */
bool safe_packet_copy(void* dest, size_t dest_size, const void* src, int packet_len) {
    // 1. Check for negative length (Prevention of Sign Extension)
    if (packet_len < 0) {
        std::cerr << "Error: Negative packet length detected.\n";
        return false;
    }

    // 2. Cast to unsigned now that we know it's non-negative
    size_t unsigned_len = static_cast<size_t>(packet_len);

    // 3. Bounds Check: Ensure we don't exceed the destination buffer
    if (unsigned_len > dest_size) {
        std::cerr << "Error: Packet length exceeds buffer capacity.\n";
        return false;
    }

    // 4. Perform the copy safely
    std::memcpy(dest, src, unsigned_len);
    return true;
}

int main() {
    char buffer[128];
    const char* raw_data = "Sensitive Packet Data";
    int malicious_len = -1; // Simulated attack or corrupt data

    if (!safe_packet_copy(buffer, sizeof(buffer), raw_data, malicious_len)) {
        std::cout << "Copy blocked to prevent memory corruption.\n";
    }

    return 0;
}