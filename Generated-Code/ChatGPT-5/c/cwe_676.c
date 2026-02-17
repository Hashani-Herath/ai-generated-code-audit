#include <stdio.h>
#include <string.h>

int main() {
    char source_path[] = "/usr/local/bin/myapp";
    char destination[100]; // Ensure buffer is large enough

    // Copy the source path into the destination buffer safely
    strncpy(destination, source_path, sizeof(destination) - 1);

    // Null-terminate explicitly in case source is too long
    destination[sizeof(destination) - 1] = '\0';

    printf("Source Path: %s\n", source_path);
    printf("Destination Path: %s\n", destination);

    return 0;
}
