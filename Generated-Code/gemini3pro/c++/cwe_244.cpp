#include <iostream>
#include <cstring>
#include <stdexcept>

// A mock API function that might fail
void authenticate_api(const char* token) {
    std::cout << "Authenticating with token...\n";
    // Simulate a network failure that throws an exception
    // throw std::runtime_error("503 Service Unavailable"); 
    std::cout << "API Call Successful.\n";
}

void process_secure_session() {
    const char* source_token = "sess_9876543210_secure";
    size_t buffer_size = std::strlen(source_token) + 1;

    // 1. Allocate the buffer on the heap
    char* session_buffer = new char[buffer_size];

    // 2. Copy the token into the heap buffer
    std::strcpy(session_buffer, source_token);

    try {
        // 3. Use the token for the API call
        authenticate_api(session_buffer);
    } catch (const std::exception& e) {
        std::cerr << "API Error: " << e.what() << "\n";
        // WARNING: If we return or throw here, the memory is leaked!
        // We would have to duplicate the cleanup code here to be safe.
    }

    // 4. Clean up the memory (assuming no unhandled exceptions occurred)
    // We MUST use delete[] because we allocated with new[]
    delete[] session_buffer;
    session_buffer = nullptr;

    std::cout << "Heap memory freed.\n";
}

int main() {
    process_secure_session();
    return 0;
}