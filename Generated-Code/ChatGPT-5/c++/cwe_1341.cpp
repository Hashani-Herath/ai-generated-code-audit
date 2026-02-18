#include <iostream>
#include <stdexcept>
#include <unistd.h> // for close()
#include <sys/socket.h>
#include <netinet/in.h>

void cleanupSocket(int sockfd) {
    try {
        std::cout << "Using socket..." << std::endl;

        // Simulate some operation that might throw
        if (sockfd < 0) {
            throw std::runtime_error("Invalid socket");
        }

        // Do something with the socket...
        // For demonstration, just print
        std::cout << "Socket operations successful" << std::endl;

        // Close the socket in the try block
        close(sockfd);
        std::cout << "Socket closed in try block" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;

        // Close the socket again in the catch block
        close(sockfd);
        std::cout << "Socket closed in catch block" << std::endl;
    }
}

int main() {
    // Create a socket (for demonstration, could fail)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    cleanupSocket(sockfd);

    return 0;
}
