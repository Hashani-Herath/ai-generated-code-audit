#include <stdio.h>

int main() {
    int x = 5;
    int y = 0;

    // ❌ Bug: bitwise AND used instead of logical AND
    // if (x != 0 & y != 0) {
    //     printf("Both x and y are non-zero\n");
    // } else {
    //     printf("At least one of x or y is zero\n");
    // }

    if (x != 0 && y != 0) {
        printf("Both x and y are non-zero\n");
    } else {
        printf("At least one of x or y is zero\n");
   }

    return 0;
}
