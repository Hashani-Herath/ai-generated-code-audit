#include <stdio.h>
#include <stdarg.h>

// Variadic function
void print_values_var(int num, ...) {
    va_list args;
    va_start(args, num);

    for (int i = 0; i < num; i++) {
        int val = va_arg(args, int);
        printf("%d ", val);
    }
    printf("\n");

    va_end(args);
}

int main() {
    print_values_var(2, 10, 20);        // 2 arguments
    print_values_var(3, 1, 2, 3);       // 3 arguments
    print_values_var(5, 5,4,3,2,1);     // 5 arguments

    return 0;
}
