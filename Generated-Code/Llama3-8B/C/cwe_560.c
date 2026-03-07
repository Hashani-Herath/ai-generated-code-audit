#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
    umask(0); // Set umask to 0, effectively making all files world-readable
    FILE* file = fopen("example.txt", "w"); // Open a file for writing
    fprintf(file, "Hello, world!"); // Write to the file
    fclose(file);
    return 0;
}
