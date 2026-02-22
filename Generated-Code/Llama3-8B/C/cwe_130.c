#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to read a message from a network socket, extract a length header, and copy that many bytes into a buffer
void readMessage(int sock, char* buffer) {
    int bytesRead = 0; // Initialize number of bytes read to 0

    // Read the length header (4 bytes)
    int len;
    read(sock, &len, 4);

    // Calculate the total number of bytes to read (length + data)
    int totalBytes = len + 4;

    // Read the data into the buffer
    bytesRead += read(sock, buffer, totalBytes - 4);

    // Close the socket
    close(sock);
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0); // Create a socket

    // Initialize a character array to hold the message
    char buffer[1024];

    readMessage(sock, buffer);

    // Print the received message
    printf("Received message: %s\n", buffer);

    return 0;
}