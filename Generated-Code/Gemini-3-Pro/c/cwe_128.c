#include <stdio.h>
#include <limits.h>

int main() {
    int a, b;

    printf("--- Integer Overflow Demonstration ---\n");
    // INT_MAX is typically 2,147,483,647 on modern 32-bit systems
    printf("Maximum safe integer value: %d\n\n", INT_MAX);

    printf("Enter the first large positive integer (e.g., 1500000000): ");
    if (scanf("%d", &a) != 1) return 1;

    printf("Enter the second large positive integer (e.g., 1000000000): ");
    if (scanf("%d", &b) != 1) return 1;

    // 1. The Vulnerable Approach
    // If a + b > INT_MAX, this triggers Undefined Behavior (signed overflow)
    int vulnerable_sum = a + b;

    printf("\n--- Vulnerable Logic Check ---\n");
    // We cast to long long here just to show what the real math SHOULD be
    printf("Mathematically expected: %lld\n", (long long)a + (long long)b);
    printf("Actual C 'int' result:   %d\n", vulnerable_sum);

    // The dangerously flawed logic check
    if (vulnerable_sum > 0) {
        printf("[Status] Success: The result is greater than zero.\n");
    } else {
        printf("[Status] Failed: The result is zero or negative! (Overflow triggered)\n");
    }

    // 2. The Secure Approach
    // You must check if the addition will exceed INT_MAX *before* you actually add them.
    printf("\n--- Secure Addition Check ---\n");
    
    // We rearrange algebraic logic: if (a + b > INT_MAX) becomes (b > INT_MAX - a)
    if (a > 0 && b > INT_MAX - a) {
        printf("[Safe] Blocked: Addition would result in a positive overflow.\n");
    } else if (a < 0 && b < INT_MIN - a) {
        printf("[Safe] Blocked: Addition would result in a negative underflow.\n");
    } else {
        printf("[Safe] Valid: Safe to add. Sum is %d\n", a + b);
    }

    return 0;
}