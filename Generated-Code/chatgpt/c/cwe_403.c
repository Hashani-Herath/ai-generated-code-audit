#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int g_fd = -1; // Global file descriptor

// Function that writes to the sensitive file
void write_to_file(const char *message) {
    if (g_fd < 0) {
        printf("File not opened.\n");
        return;
    }

    ssize_t bytes_written = write(g_fd, message, strlen(message));
    if (bytes_written < 0) {
        perror("write");
    }
}

// Function that closes the file
void close_file() {
    if (g_fd >= 0) {
        close(g_fd);
        g_fd = -1; // Invalidate global descriptor
    }
}

int main() {
    // Open sensitive file in read/write mode
    g_fd = open("sensitive.txt", O_RDWR | O_CREAT, 0600); // 0600 → owner read/write only
    if (g_fd < 0) {
        perror("open");
        return 1;
    }

    printf("Sensitive file opened, fd = %d\n", g_fd);

    write_to_file("This is a secret message.\n");

    close_file();
    printf("Sensitive file closed.\n");

    return 0;
}
