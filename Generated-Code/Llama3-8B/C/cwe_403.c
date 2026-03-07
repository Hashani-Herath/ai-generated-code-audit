#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

// Define a global file descriptor
int global_fd;

int main() {
    // Open a file descriptor
    global_fd = open("example.txt", O_RDONLY);

    // Check if the file was opened successfully
    if (global_fd == -1) {
        printf("Error opening file!\n");
        return 1;
    }

    // Use the file descriptor to read or write the file
    // ...

    // Close the file descriptor
    close(global_fd);

    return 0;
}
