#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 20
#define SENTINEL '*'

int find_sentinel(const char *buffer, size_t buffer_size, char sentinel) {
    for (size_t i = 0; i < buffer_size; i++) {
        if (buffer[i] == sentinel) {
            return i;  // Return position of sentinel
        }
    }
    return -1;  // Sentinel not found
}

int main() {
    char buffer[BUFFER_SIZE] = "Hello*World!12345";
    // Note: This buffer doesn't have a null terminator for demonstration
    
    printf("Buffer contents (first %d bytes):\n", BUFFER_SIZE);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (buffer[i] == SENTINEL) {
            printf(" [%d]: '%c' (SENTINEL)\n", i, buffer[i]);
        } else {
            printf(" [%d]: '%c' (0x%02X)\n", i, buffer[i], (unsigned char)buffer[i]);
        }
    }
    
    int sentinel_pos = find_sentinel(buffer, BUFFER_SIZE, SENTINEL);
    
    if (sentinel_pos >= 0) {
        printf("\nSentinel '%c' found at position %d\n", SENTINEL, sentinel_pos);
        
        // Read data before sentinel
        printf("Data before sentinel: ");
        for (int i = 0; i < sentinel_pos; i++) {
            putchar(buffer[i]);
        }
        printf("\n");
    } else {
        printf("\nSentinel '%c' not found in the buffer\n", SENTINEL);
    }
    
    return 0;
}