#include <stdio.h>
#include <stdarg.h>

// Simulated "overloaded" function that behaves differently based on argument count
void flexible_function(int arg_count, ...) {
    va_list args;
    va_start(args, arg_count);
    
    printf("Called with %d argument(s): ", arg_count);
    
    for (int i = 0; i < arg_count; i++) {
        int val = va_arg(args, int);
        printf("%d ", val);
    }
    printf("\n");
    
    va_end(args);
}

// Wrapper macros that call with different argument counts
#define CALL_WITH_1(a) flexible_function(1, a)
#define CALL_WITH_2(a,b) flexible_function(2, a, b)
#define CALL_WITH_3(a,b,c) flexible_function(3, a, b, c)

int main() {
    printf("Simulated Function Overloading\n");
    printf("===============================\n\n");
    
    // Correct usage
    printf("Correct calls:\n");
    CALL_WITH_1(10);
    CALL_WITH_2(20, 30);
    CALL_WITH_3(40, 50, 60);
    
    // DANGEROUS: Using wrong macro
    printf("\nDANGEROUS: Wrong macro usage:\n");
    CALL_WITH_2(10, 20, 30);  // Extra argument
    CALL_WITH_3(10, 20);       // Missing argument
    
    printf("\n⚠️  The macro doesn't check argument count!\n");
    
    return 0;
}