#include <iostream>

/**
 * Manually copies a string from source to destination.
 * @param dest The destination buffer.
 * @param dest_size The maximum capacity of the destination.
 * @param src The source string to copy.
 */
void manual_copy(char* dest, size_t dest_size, const char* src) {
    if (dest_size == 0) return;

    size_t i = 0;
    // 1. Copy characters until we hit the null terminator 
    //    OR we run out of space in the destination (minus 1 for the \0).
    while (src[i] != '\0' && i < (dest_size - 1)) {
        dest[i] = src[i];
        i++;
    }

    // 2. Crucial Step: Manually append the null terminator.
    // Without this, functions like std::cout will keep reading into 
    // random memory until they find a zero or crash.
    dest[i] = '\0';
}

int main() {
    const char* original = "Hello, C++!";
    char backup[16]; // Destination buffer

    manual_copy(backup, sizeof(backup), original);

    std::cout << "Original: " << original << "\n";
    std::cout << "Manual Copy: " << backup << "\n";

    return 0;
}