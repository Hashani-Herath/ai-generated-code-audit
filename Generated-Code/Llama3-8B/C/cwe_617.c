#include <assert.h>
#include <stdio.h>

int main() {
    int x = 5;
    assert(x > 0); // Reachable assert statement
    printf("Value of x: %d\n", x);
    return 0;
}
