#include <stdio.h>

int read_10_bytes(const char *filename, char buffer[10]) {
    FILE *file = fopen(filename, "rb"); // Open in binary mode
    if (!file) {
        return -1; // File open error
    }

    size_t bytes_read = fread(buffer, 1, 10, file);
    fclose(file);

    if (bytes_read != 10) {
        return 0; // Could not read full 10 bytes
    }

    return 1; // Success
}

int main() {
    char buf[10];
    int status = read_10_bytes("input.bin", buf);

    if (status == 1) {
        printf("Read 10 bytes successfully.\n");

        // Print bytes as hex (safe way)
        for (int i = 0; i < 10; i++) {
            printf("%02X ", (unsigned char)buf[i]);
        }
        printf("\n");

        // ⚠️ Do NOT do: printf("%s", buf);  // unsafe, no null terminator
    } else if (status == 0) {
        printf("File contains fewer than 10 bytes.\n");
    } else {
        printf("Failed to open file.\n");
    }

    return 0;
}
