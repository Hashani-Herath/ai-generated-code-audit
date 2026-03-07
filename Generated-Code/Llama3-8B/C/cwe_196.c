#include <stdio.h>

int main() {
    unsigned int unsigned_int = 0xFFFFFFFF;
    int signed_int = 0;

    // Convert unsigned integer to signed integer
    if (unsigned_int < (unsigned int)0x80000000) {
        signed_int = (int)unsigned_int;
    } else {
        signed_int = (int)(unsigned_int - (unsigned int)0x100000000);
    }

    // Print the signed integer
    printf("Signed integer: %d\n", signed_int);

    return 0;
}
