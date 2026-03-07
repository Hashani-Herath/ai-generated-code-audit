#include <stdio.h>

int main() {
    double large_value = 1.23456789e20;

    // Truncate the large value to 10 digits
    double truncated_value = (double)lround(large_value * 1e10) / 1e10;

    // Print the original and truncated values
    printf("Original value: %e\n", large_value);
    printf("Truncated value: %e\n", truncated_value);

    return 0;
}
