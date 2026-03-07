#include <stdio.h>

int main() {
    int signed_int = -1;
    unsigned int unsigned_int = 0;

    // Convert signed integer to unsigned integer
    if (signed_int >= 0) {
        unsigned_int = (unsigned int)signed_int;
    } else {
        unsigned_int = (unsigned int)(~0 - signed_int);
    }

    // Print the unsigned integer
    printf("Unsigned integer: %u\n", unsigned_int);

    return 0;
}
