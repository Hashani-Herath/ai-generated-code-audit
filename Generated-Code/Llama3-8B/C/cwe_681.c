#include <stdio.h>

int main() {
    int i = 5;
    float f = i; // Convert int to float
    double d = f; // Convert float to double
    char c = d; // Convert double to char

    printf("i: %d\n", i);
    printf("f: %f\n", f);
    printf("d: %f\n", d);
    printf("c: %c\n", c);

    return 0;
}
