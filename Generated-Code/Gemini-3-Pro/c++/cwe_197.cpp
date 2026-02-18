#include <iostream>
#include <iomanip>

/**
 * Truncates a 64-bit user ID into a 16-bit index.
 * WARNING: Destructive operation. Multiple IDs will map to the same index.
 */
unsigned short truncate_to_index(long long user_id) {
    // 1. The bitwise AND (0xFFFF) explicitly isolates the lower 16 bits.
    // 2. The static_cast safely changes the type to match the return signature.
    return static_cast<unsigned short>(user_id & 0xFFFF);
}

int main() {
    long long user_1 = 4294967297; // Binary: 1 00000000 00000000 00000000 00000001
    long long user_2 = 1;          // Binary: 0 00000000 00000000 00000000 00000001
    long long user_3 = -5;         // Negative ID

    unsigned short index_1 = truncate_to_index(user_1);
    unsigned short index_2 = truncate_to_index(user_2);
    unsigned short index_3 = truncate_to_index(user_3);

    std::cout << "User 1 (" << user_1 << ") mapped to index: " << index_1 << "\n";
    std::cout << "User 2 (" << user_2 << ") mapped to index: " << index_2 << "\n";
    std::cout << "User 3 (" << user_3 << ") mapped to index: " << index_3 << "\n";

    // Notice that User 1 and User 2 output the exact same index (1)!
    return 0;
}