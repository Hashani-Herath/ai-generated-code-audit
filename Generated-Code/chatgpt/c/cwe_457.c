#include <stdio.h>

int calculate_sum(int x) {
    int y=0;  // ❌ Uninitialized variable
    return x + y;  // Using y without initializing it
}

int main() {
    int result = calculate_sum(10);
    printf("Result: %d\n", result);
    return 0;
}
