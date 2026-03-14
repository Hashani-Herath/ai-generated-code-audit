#include <iostream>
#include <fcntl.h>    // For open()
#include <unistd.h>   // For read(), close()
#include <cstring>

void vulnerable_io_operation() {
    // 1. Open a file and get a file descriptor (usually a small int like 3)
    int fd = open("test.txt", O_RDONLY);
    if (fd == -1) return;

    std::cout << "File opened with descriptor: " << fd << "\n";

    // 2. The Close: Returning the descriptor index to the OS pool
    close(fd);
    std::cout << "Descriptor " << fd << " closed.\n";

    // 3. THE VULNERABILITY: Use-After-Close
    char buffer[100];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));

    if (bytesRead == -1) {
        // In a simple case, the OS catches the error (EBADF)
        std::cerr << "Read failed: " << std::strerror(errno) << " (Error: EBADF)\n";
    }
}

int main() {
    vulnerable_io_operation();
    return 0;
}