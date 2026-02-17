#include <stdio.h>
#include <limits.h>

int main() {
    short s;
    long l;

    printf("Enter a signed short integer (%d to %d): ", SHRT_MIN, SHRT_MAX);
    if (scanf("%hd", &s) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Assign short to long (automatic promotion)
    l = s;

    printf("Signed short: %hd\n", s);
    printf("Assigned to signed long: %ld\n", l);

    return 0;
}
