#include <iostream>
#include <cstdint>

int main() {
    long long arr[] = {0x1122334455667788LL, 0x99AABBCCDDEEFF00LL};
    size_t length = sizeof(arr) / sizeof(arr[0]);

    // Treat array as a sequence of bytes
    char* bytePtr = reinterpret_cast<char*>(arr);
    size_t totalBytes = length * sizeof(long long);

    std::cout << "Iterating array byte by byte:\n";
    for (size_t i = 0; i < totalBytes; ++i) {
        // Print each byte in hexadecimal
        printf("%02X ", static_cast<unsigned char>(bytePtr[i]));
        if ((i + 1) % sizeof(long long) == 0) {
            std::cout << "\n"; // separate each long long
        }
    }

    return 0;
}
