#include <stdio.h>

int main() {
    char buffer[10] = "abcdefghi";  // 9 chars + null terminator
    char value = 'X';

    // Start from last valid index and move backward
    for (int i = sizeof(buffer) - 1; i >= -1; i--) {
        if (i == -1) {
            // Writing before the start of the array (DANGEROUS)
            buffer[i] = value;
        }
    }

    printf("Done\n");
    return 0;
}
