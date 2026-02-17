#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 10

/**
 * Reads exactly 10 bytes from a file into a 10-byte buffer
 * DOES NOT add a null terminator
 * 
 * @param filename Name of the file to read
 * @param buffer 10-byte buffer to store the data
 * @return Number of bytes actually read, or -1 on error
 */
int read_10_bytes_no_null(const char *filename, char buffer[BUFFER_SIZE]) {
    FILE *file;
    size_t bytes_read;
    
    // Open file in binary mode to avoid newline conversions
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    
    // Read exactly 10 bytes
    bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
    
    fclose(file);
    
    // Return number of bytes read (could be less than 10 at EOF)
    return bytes_read;
}

int main() {
    char buffer[BUFFER_SIZE];  // No null terminator guaranteed!
    int bytes_read;
    int i;
    
    printf("Reading 10 bytes from file without null terminator\n");
    printf("==================================================\n");
    
    bytes_read = read_10_bytes_no_null("testfile.bin", buffer);
    
    if (bytes_read < 0) {
        printf("Failed to read file\n");
        return 1;
    }
    
    printf("Successfully read %d bytes\n", bytes_read);
    
    // Display bytes in various formats
    printf("\nByte values:\n");
    printf("Index | Decimal | Hexadecimal | Character\n");
    printf("------|---------|-------------|----------\n");
    
    for (i = 0; i < bytes_read; i++) {
        unsigned char c = buffer[i];
        printf("  %2d  |   %3d   |    0x%02X    | ", i, c, c);
        
        // Print character if printable, otherwise show dot
        if (c >= 32 && c <= 126) {
            printf("'%c'", c);
        } else {
            printf(" . ");
        }
        printf("\n");
    }
    
    // WARNING: Don't treat as string!
    printf("\nIf treated as string: \"");
    for (i = 0; i < bytes_read; i++) {
        putchar(buffer[i]);
    }
    printf("\"\n");
    printf("(This may show garbage or crash if buffer doesn't contain valid characters)\n");
    
    return 0;
}