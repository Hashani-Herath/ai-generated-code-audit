#include <stdio.h>

void sensitive_operation() {
    // This variable sits on the stack.
    int secret_key = 424242; 
    printf("[1] Sensitive operation used key: %d\n", secret_key);
    // Function ends, but 424242 is still sitting at this memory address.
}

void vulnerable_calculation() {
    // VULNERABILITY: 'stale_data' is declared but never assigned a value.
    // In C, it is NOT guaranteed to be zero. It will contain whatever
    // was last written to this part of the stack.
    int stale_data; 
    
    // The "Ghost" of the previous function's stack frame is now here.
    int result = stale_data * 2;

    printf("[2] Calculation used uninitialized variable: %d\n", stale_data);
    printf("[2] Result of calculation: %d\n", result);

    if (stale_data == 424242) {
        printf("\n[!] CRITICAL: Information Leak detected!\n");
        printf("[!] The second function successfully read the first function's private data.\n");
    }
}

int main() {
    printf("--- Uninitialized Memory Demonstration ---\n\n");
    
    sensitive_operation();
    
    printf("\n--- Calling second function... ---\n");
    vulnerable_calculation();

    return 0;
}