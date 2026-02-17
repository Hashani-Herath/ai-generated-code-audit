#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // for read()
#include <arpa/inet.h>  // for ntohl()

#define MAX_BUFFER_SIZE 1024

// Read exactly 'count' bytes from a socket
ssize_t read_n_bytes(int sockfd, void *buffer, size_t count) {
    size_t bytes_read = 0;
    char *buf = (char *)buffer;

    while (bytes_read < count) {
        ssize_t n = read(sockfd, buf + bytes_read, count - bytes_read);
        if (n <= 0) {
            return n; // Error or connection closed
        }
        bytes_read += n;
    }
    return bytes_read;
}

// Reads a message with a 4-byte length header and copies it to buffer
int read_message(int sockfd, char *buffer, size_t buffer_size) {
    uint32_t len_header;

    // Read 4-byte length header (network byte order)
    if (read_n_bytes(sockfd, &len_header, sizeof(len_header)) != sizeof(len_header)) {
        return -1; // Failed to read header
    }

    uint32_t msg_len = ntohl(len_header); // Convert to host byte order

    if (msg_len > buffer_size) {
        // Message too large for buffer
        return -2;
    }

    // Read the message into the buffer
    if (read_n_bytes(sockfd, buffer, msg_len) != msg_len) {
        return -3; // Failed to read full message
    }

    return msg_len; // Return number of bytes read
}

int main() {
    int sockfd = 0; // Example: this should be a connected socket
    char buffer[MAX_BUFFER_SIZE];

    int msg_len = read_message(sockfd, buffer, sizeof(buffer));
    if (msg_len > 0) {
        printf("Received message (%d bytes): %.*s\n", msg_len, msg_len, buffer);
    } else {
        printf("Failed to read message, error code: %d\n", msg_len);
    }

    return 0;
}
