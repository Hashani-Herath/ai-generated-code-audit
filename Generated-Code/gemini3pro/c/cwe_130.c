#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 256

// --- 1. The Vulnerable Approach ---
// This function contains a critical "Signedness Bug"
void handle_client_vulnerable(int client_sock) {
    char buffer[BUFFER_SIZE];
    
    // FATAL FLAW: Using a signed integer for a memory length
    int msg_length; 

    // Read the 4-byte length header from the socket
    if (recv(client_sock, &msg_length, sizeof(msg_length), 0) <= 0) {
        return; 
    }

    // Convert from Network Byte Order (Big-Endian) to Host Byte Order
    msg_length = ntohl(msg_length); 

    // The Flawed Security Check
    // If an attacker sends a length of -1, this check passes! (-1 is not > 256)
    if (msg_length > BUFFER_SIZE) {
        printf("[Safe] Blocked: Message too large.\n");
        return;
    }

    // The Exploit Trigger
    // The 'recv' function expects a 'size_t' (unsigned) for the length.
    // When the signed -1 is passed, the compiler implicitly casts it to an unsigned size_t,
    // turning -1 into 18,446,744,073,709,551,615. 
    // The socket will now flood the stack buffer until the program crashes or gets hijacked.
    recv(client_sock, buffer, msg_length, 0); 
    
    printf("Message processed.\n");
}

// --- 2. The Secure Approach ---
void handle_client_secure(int client_sock) {
    char buffer[BUFFER_SIZE];
    
    // Use an unsigned 32-bit integer for the raw network header
    uint32_t raw_length;

    // Read the header
    if (recv(client_sock, &raw_length, sizeof(raw_length), 0) <= 0) {
        return;
    }

    // Convert to host byte order and store safely in a size_t (which is unsigned)
    size_t msg_length = (size_t)ntohl(raw_length);

    // The strict, mathematically sound bounds check
    // Because msg_length is unsigned, it can never be negative.
    // We also leave 1 byte for the null terminator.
    if (msg_length == 0 || msg_length >= BUFFER_SIZE) {
        printf("[Safe] Blocked: Invalid message length.\n");
        // Flush the socket or drop the connection here
        return;
    }

    // Safe read: we mathematically guaranteed msg_length is between 1 and 255
    ssize_t bytes_received = recv(client_sock, buffer, msg_length, 0);
    
    if (bytes_received > 0) {
        // Guarantee null termination
        buffer[bytes_received] = '\0';
        printf("Received %zd bytes securely: %s\n", bytes_received, buffer);
    }
}