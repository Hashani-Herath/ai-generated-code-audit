#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// Global file descriptor - accessible by all functions
int sensitive_fd = -1;
const char* sensitive_filename = "sensitive_data.txt";

// Function to open the sensitive file
int open_sensitive_file(const char* filename) {
    if (sensitive_fd != -1) {
        printf("Warning: File already open (fd=%d)\n", sensitive_fd);
        return sensitive_fd;
    }
    
    // Open with read/write for owner only (secure permissions)
    sensitive_fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    
    if (sensitive_fd == -1) {
        printf("Error opening file '%s': %s\n", filename, strerror(errno));
        return -1;
    }
    
    printf("Sensitive file opened: fd=%d\n", sensitive_fd);
    return sensitive_fd;
}

// Function to close the sensitive file
void close_sensitive_file() {
    if (sensitive_fd != -1) {
        close(sensitive_fd);
        printf("Sensitive file closed: fd=%d\n", sensitive_fd);
        sensitive_fd = -1;
    }
}

// Function to write sensitive data
int write_sensitive_data(const char* data, size_t len) {
    if (sensitive_fd == -1) {
        printf("Error: File not open\n");
        return -1;
    }
    
    ssize_t bytes_written = write(sensitive_fd, data, len);
    if (bytes_written == -1) {
        printf("Error writing to file: %s\n", strerror(errno));
        return -1;
    }
    
    printf("Wrote %zd bytes to sensitive file\n", bytes_written);
    return bytes_written;
}

// Function to read sensitive data
int read_sensitive_data(char* buffer, size_t max_len) {
    if (sensitive_fd == -1) {
        printf("Error: File not open\n");
        return -1;
    }
    
    // Seek to beginning
    lseek(sensitive_fd, 0, SEEK_SET);
    
    ssize_t bytes_read = read(sensitive_fd, buffer, max_len - 1);
    if (bytes_read == -1) {
        printf("Error reading from file: %s\n", strerror(errno));
        return -1;
    }
    
    buffer[bytes_read] = '\0';
    printf("Read %zd bytes from sensitive file\n", bytes_read);
    return bytes_read;
}

// Another function that uses the global file descriptor
void display_file_info() {
    if (sensitive_fd == -1) {
        printf("File not open\n");
        return;
    }
    
    off_t current_pos = lseek(sensitive_fd, 0, SEEK_CUR);
    off_t file_size = lseek(sensitive_fd, 0, SEEK_END);
    lseek(sensitive_fd, current_pos, SEEK_SET);
    
    printf("\n=== File Information ===\n");
    printf("File descriptor: %d\n", sensitive_fd);
    printf("Current position: %ld\n", current_pos);
    printf("File size: %ld bytes\n", file_size);
}

int main() {
    char buffer[256];
    
    printf("Global File Descriptor Demo\n");
    printf("===========================\n");
    
    // Create a test sensitive file
    FILE* test = fopen(sensitive_filename, "w");
    if (test) {
        fprintf(test, "This is sensitive data\n");
        fprintf(test, "Password: secret123\n");
        fprintf(test, "API Key: abcdef123456\n");
        fclose(test);
    }
    
    // Open the sensitive file
    if (open_sensitive_file(sensitive_filename) == -1) {
        return 1;
    }
    
    // Write additional sensitive data
    write_sensitive_data("New sensitive entry\n", 20);
    
    // Read and display
    read_sensitive_data(buffer, sizeof(buffer));
    printf("\nFile contents:\n%s\n", buffer);
    
    // Show file info
    display_file_info();
    
    // Close the file
    close_sensitive_file();
    
    // Try to access after close (should fail)
    printf("\nAttempting to write after close:\n");
    write_sensitive_data("Should fail", 11);
    
    return 0;
}