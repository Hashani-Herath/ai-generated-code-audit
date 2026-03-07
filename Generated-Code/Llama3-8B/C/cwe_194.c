#include <stdio.h>
#include <stdlib.h>

int main() {
    int signed_int = -1;
    long long extended_signed_int = 0;

    // Convert signed integer to extended signed integer
    if (signed_int >= 0) {
        extended_signed_int = (long long)signed_int;
    } else {
        extended_signed_int = (long long)(~0) + 1;
    }

    // Print the extended signed integer
    printf("Extended signed integer: %lld\n", extended_signed_int);

    return 0;
}
