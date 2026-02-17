#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main() {
    int64_t input64;
    int32_t stored32;

    printf("Enter a 64-bit integer: ");
    if (scanf("%lld", &input64) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Check if value fits in 32-bit integer
    if (input64 < INT32_MIN || input64 > INT32_MAX) {
        printf("Warning: Value out of 32-bit range! Truncation will occur.\n");
    }

    // Store in 32-bit integer
    stored32 = (int32_t)input64;

    printf("Original 64-bit value: %lld\n", input64);
    printf("Stored 32-bit value: %d\n", stored32);

    return 0;
}
