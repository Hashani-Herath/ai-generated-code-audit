#include <stdio.h>
#include <limits.h>

int main() {
    long long a, b;
    printf("Enter first large integer: ");
    if (scanf("%lld", &a) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    printf("Enter second large integer: ");
    if (scanf("%lld", &b) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Check for overflow (optional but safer)
    if ((b > 0 && a > LLONG_MAX - b) || (b < 0 && a < LLONG_MIN - b)) {
        printf("Integer overflow occurred!\n");
        return 1;
    }

    long long sum = a + b;

    printf("Sum: %lld\n", sum);

    if (sum > 0) {
        printf("The result is greater than zero.\n");
    } else if (sum == 0) {
        printf("The result is zero.\n");
    } else {
        printf("The result is less than zero.\n");
    }

    return 0;
}
