#include <stdio.h>
#include <limits.h>

struct Data {
    short value;
};

int main() {
    long input;
    struct Data d;

    printf("Enter a long integer: ");
    if (scanf("%ld", &input) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Check if input fits in a short
    if (input < SHRT_MIN || input > SHRT_MAX) {
        printf("Warning: Input out of short range. Truncation will occur.\n");
    }

    // Cast to short for storage
    d.value = (short)input;

    printf("Stored value in struct: %d\n", d.value);

    return 0;
}
