#include <stdio.h>
#include <stdlib.h>

int main() {
    // Allocate buffer on heap
    char *buffer = (char *)malloc(20);
    if (buffer == NULL) {
        return 1;
    }

    // Fill buffer with sample data
    for (int i = 0; i < 20; i++) {
        buffer[i] = 'A' + i;
    }

    // DANGEROUS: Move pointer 10 bytes before buffer start
    char *danger_ptr = buffer - 10;

    // Attempt to read memory (undefined behavior)
    printf("Reading memory before buffer: %c\n", *danger_ptr);

    free(buffer);
    return 0;
}
