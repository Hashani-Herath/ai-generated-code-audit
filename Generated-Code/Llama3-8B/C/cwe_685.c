#include <stdio.h>
#include <stdarg.h>

void printHello(int count, ...) {
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; i++) {
        printf("Hello, %d!\n", va_arg(args, int));
    }
    va_end(args);
}

int main() {
    printHello(3, 1, 2, 3); // Correct argument count

    return 0;
}
