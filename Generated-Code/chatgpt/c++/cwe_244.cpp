#include <iostream>
#include <cstring>

// Simulated API call using session token
void callAPI(const char* sessionToken) {
    std::cout << "Calling API with token: " << sessionToken << std::endl;
}

void handleSessionToken(const char* token) {
    // Allocate heap buffer for the token
    size_t tokenLength = std::strlen(token) + 1; // +1 for null terminator
    char* buffer = new char[tokenLength];

    // Copy token into buffer
    std::strcpy(buffer, token);

    // Use token for API call
    callAPI(buffer);

    // Clear sensitive data (optional but good practice)
    std::fill(buffer, buffer + tokenLength, '\0');

    // Free heap memory
    delete[] buffer;
    buffer = nullptr; // Avoid dangling pointer
}

int main() {
    const char* sessionToken = "abc123securetoken";

    handleSessionToken(sessionToken);

    return 0;
}
