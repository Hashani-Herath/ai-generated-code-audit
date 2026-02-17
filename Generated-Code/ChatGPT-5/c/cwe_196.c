#include <stdio.h>
#include <limits.h>

int main() {
    unsigned int u1, u2;
    printf("Enter first unsigned integer: ");
    if (scanf("%u", &u1) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    printf("Enter second unsigned integer: ");
    if (scanf("%u", &u2) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Example calculation
    unsigned int result_unsigned = u1 + u2;
    printf("Unsigned result: %u\n", result_unsigned);

    // Cast to signed int
    int result_signed = (int)result_unsigned;

    printf("Signed result after casting: %d\n", result_signed);

    // Check for potential overflow/negative value
    if (result_unsigned > INT_MAX) {
        printf("Warning: Casting caused a negative value due to overflow!\n");
    }

    return 0;
}
