#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <stdint.h>

#define HEADER_SIZE 4
#define DEFAULT_TIMEOUT_SEC 5
#define MAX_MESSAGE_SIZE 65536

typedef struct {
    int socket_fd;
    int timeout_seconds;
    size_t max_message_size;
} SocketReader;

/**
 * Sets socket timeout
 */
int set_socket_timeout(int socket_fd, int seconds) {
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        return -1;
    }
    return 0;
}

/**
 * Reads exactly n bytes from socket (with timeout handling)
 */
ssize_t read_exactly(int socket_fd, void *buffer, size_t n) {
    size_t total_read = 0;
    ssize_t bytes_read;
    
    while (total_read < n) {
        bytes_read = recv(socket_fd, 
                          (char*)buffer + total_read, 
                          n - total_read, 
                          0);
        
        if (bytes_read < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Socket timeout: no data received for %d seconds\n", 
                       DEFAULT_TIMEOUT_SEC);
            } else {
                perror("recv failed");
            }
            return -1;
        }
        
        if (bytes_read == 0) {
            printf("Connection closed by peer\n");
            return total_read;  // Return partial read
        }
        
        total_read += bytes_read;
    }
    
    return total_read;
}

/**
 * Reads message with header and validates length
 */
ssize_t read_message_safe(SocketReader *reader, unsigned char *buffer, size_t buffer_size) {
    uint32_t message_length;
    ssize_t result;
    
    if (!reader || !buffer) {
        printf("Invalid parameters\n");
        return -1;
    }
    
    // Set timeout
    if (set_socket_timeout(reader->socket_fd, reader->timeout_seconds) < 0) {
        return -1;
    }
    
    // Read header
    result = read_exactly(reader->socket_fd, &message_length, HEADER_SIZE);
    if (result != HEADER_SIZE) {
        return -1;
    }
    
    // Convert from network byte order to host
    message_length = ntohl(message_length);
    
    printf("Received header: length = %u bytes\n", message_length);
    
    // Validate message length
    if (message_length > reader->max_message_size) {
        printf("Message length %u exceeds maximum allowed %zu\n", 
               message_length, reader->max_message_size);
        return -1;
    }
    
    if (message_length > buffer_size) {
        printf("Message length %u exceeds buffer size %zu\n", 
               message_length, buffer_size);
        return -1;
    }
    
    if (message_length == 0) {
        return 0;  // Empty message
    }
    
    // Read message body
    result = read_exactly(reader->socket_fd, buffer, message_length);
    if (result != message_length) {
        printf("Incomplete message: expected %u bytes, got %zd\n", 
               message_length, result);
        return -1;
    }
    
    return result;
}

// Example usage
int main() {
    // Create a socket (simplified - in real code you'd have proper socket setup)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // Configure reader
    SocketReader reader;
    reader.socket_fd = server_fd;
    reader.timeout_seconds = 5;
    reader.max_message_size = MAX_MESSAGE_SIZE;
    
    // Buffer for receiving messages
    unsigned char buffer[MAX_MESSAGE_SIZE];
    
    // Read message
    ssize_t bytes_received = read_message_safe(&reader, buffer, sizeof(buffer));
    
    if (bytes_received > 0) {
        printf("Successfully received %zd bytes\n", bytes_received);
        printf("Message: %.*s\n", (int)bytes_received, buffer);
    } else if (bytes_received == 0) {
        printf("Received empty message\n");
    } else {
        printf("Failed to receive message\n");
    }
    
    close(server_fd);
    return 0;
}