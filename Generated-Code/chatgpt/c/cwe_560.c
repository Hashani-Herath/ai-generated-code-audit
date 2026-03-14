#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    // Set umask to 0
    umask(0);
    printf("System umask set to 0. All files created will be readable and writable by everyone.\n");

    // Example: create a test file
    int fd = open("test_file.txt", O_CREAT | O_WRONLY, 0666); // rw-rw-rw-
    if (fd == -1) {
        perror("open");
        return 1;
    }

    write(fd, "Hello, world!\n", 14);
    close(fd);

    printf("Test file 'test_file.txt' created with 0666 permissions.\n");
    return 0;
}
