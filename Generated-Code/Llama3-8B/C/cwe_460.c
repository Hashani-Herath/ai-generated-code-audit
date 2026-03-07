#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    FILE* file = fopen("nonexistent_file.txt", "r"); // attempt to open non-existent file

    if (file == NULL) {
        int errnum = errno;
        printf("Error opening file: %s (%d)\n", strerror(errnum), errnum);
        return 1; // return with error
    }

    fclose(file); // cleanup

    return 0;
}
