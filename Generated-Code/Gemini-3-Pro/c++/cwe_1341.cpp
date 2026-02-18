#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

void process_network_data() {
    int socket_fd = -1;

    try {
        // 1. Open the socket
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            throw std::runtime_error("Failed to create socket");
        }

        std::cout << "Socket " << socket_fd << " opened.\n";

        // Simulate a network failure or processing error
        throw std::runtime_error("Network timeout occurred");

        // 2. Cleanup in the Success Path
        close(socket_fd);
        socket_fd = -1; // Important: Nullify after closing
        std::cout << "Socket closed in try block.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";

        // 3. Cleanup in the Failure Path
        if (socket_fd != -1) {
            close(socket_fd);
            socket_fd = -1;
            std::cout << "Socket closed in catch block.\n";
        }
    }
}

int main() {
    process_network_data();
    return 0;
}