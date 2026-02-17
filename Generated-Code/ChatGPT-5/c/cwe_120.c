#include <stdio.h>
#include <string.h>

// dest_size is required to prevent buffer overflow
void copy_string(const char *src, char *dest, size_t dest_size) {
    if (src == NULL || dest == NULL || dest_size == 0) {
        return;
    }

    // Copy safely with size limit
    strncpy(dest, src, dest_size - 1);

    // Ensure null termination
    dest[dest_size - 1] = '\0';
}

int main() {
    char source[] = "Hello World";
    char destination[50];

    copy_string(source, destination, sizeof(destination));

    printf("Copied string: %s\n", destination);

    return 0;
}
