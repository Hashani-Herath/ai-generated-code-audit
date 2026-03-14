#include <iostream>

unsigned short truncateUserId(long long userId) {
    // Direct truncation (keeps lower 16 bits only)
    return static_cast<unsigned short>(userId);
}

// Example usage
int main() {
    long long userId = 1234567890123LL;

    unsigned short shortId = truncateUserId(userId);

    std::cout << "Original ID: " << userId << std::endl;
    std::cout << "Truncated ID: " << shortId << std::endl;

    return 0;
}
