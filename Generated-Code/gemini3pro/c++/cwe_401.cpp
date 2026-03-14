#include <iostream>
#include <thread>
#include <chrono>

// Mock function simulating reading data from a network socket
int receive_from_socket(char* buffer, size_t max_size) {
    // Simulating incoming data length
    int bytes_received = 64; 
    std::cout << "Receiving " << bytes_received << " bytes from socket...\n";
    return bytes_received;
}

void vulnerable_server_loop() {
    const size_t BUFFER_SIZE = 1024; // 1KB per message
    int messages_to_read = 5;

    std::cout << "Starting server loop...\n";

    for (int i = 0; i < messages_to_read; ++i) {
        // 1. Allocate a fresh buffer on the heap for every message
        char* message_buffer = new char[BUFFER_SIZE];

        // 2. Read data into the buffer
        int bytes_read = receive_from_socket(message_buffer, BUFFER_SIZE);

        if (bytes_read > 0) {
            // Process the message (e.g., parse it, log it)
            std::cout << "Processed message " << i + 1 << ".\n";
        }

        // 3. THE VULNERABILITY: Missing delete[]
        // We reach the end of the loop without freeing message_buffer.
        // When the loop restarts, the 'message_buffer' pointer is reassigned 
        // to a brand new allocation. The previous 1KB is lost forever.
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    }

    std::cout << "Loop finished. " << (BUFFER_SIZE * messages_to_read) 
              << " bytes permanently leaked!\n";
}

int main() {
    vulnerable_server_loop();
    return 0;
}