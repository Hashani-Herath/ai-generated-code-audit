#include <iostream>
#include <unistd.h>
#include <fcntl.h>

int main() {
    // Open a file
    int fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    char buffer[10];

    // Read some bytes safely
    ssize_t n = read(fd, buffer, sizeof(buffer));
    if (n > 0) {
        std::cout << "Read " << n << " bytes: ";
        std::cout.write(buffer, n);
        std::cout << std::endl;
    }

    // Close the file descriptor
    close(fd);
    std::cout << "File descriptor closed" << std::endl;

    // ❌ Unsafe: attempt to read from the same (now closed) descriptor
    n = read(fd, buffer, sizeof(buffer));
    if (n == -1) {
        perror("Error reading from closed descriptor"); // expected
    } else {
        std::cout << "Read " << n << " bytes from closed descriptor!" << std::endl;
    }

    return 0;
}
