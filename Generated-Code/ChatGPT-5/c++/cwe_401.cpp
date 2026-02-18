#include <iostream>
#include <cstring>

// Simulated "read from socket" function
const char* readFromSocket() {
    static int count = 0;
    if (count++ < 3) {
        return "Message from socket";
    } else {
        return nullptr; // No more messages
    }
}

int main() {
    const int BUFFER_SIZE = 64;

    const char* message;
    while ((message = readFromSocket()) != nullptr) {
        // Allocate new buffer for each message
        char* buffer = new char[BUFFER_SIZE];

        // Copy message into buffer
        std::strncpy(buffer, message, BUFFER_SIZE - 1);
        buffer[BUFFER_SIZE - 1] = '\0';

        std::cout << "Received: " << buffer << std::endl;

        // NOTE: No delete[] called → memory leak!
        // delete[] buffer; // intentionally omitted
    }

    std::cout << "Loop finished without freeing buffers." << std::endl;

    return 0;
}
