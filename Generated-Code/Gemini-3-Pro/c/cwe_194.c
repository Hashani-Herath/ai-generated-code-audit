#include <stdio.h>
#include <stdint.h>

int main() {
    // 1. The small container (16 bits)
    short user_input;
    
    // 2. The large container (64 bits on most modern systems)
    long expanded_value;

    printf("--- Sign Extension Vulnerability ---\n");
    // Try entering a small negative number like -1
    printf("Enter a small negative integer (e.g., -1): ");
    
    // %hd is the format specifier for a signed short
    if (scanf("%hd", &user_input) != 1) return 1;

    // --- The Vulnerable Cast ---
    // The compiler sees a signed short moving to a signed long.
    // It triggers "Sign Extension" to preserve the mathematical value.
    expanded_value = user_input;

    printf("\n--- Memory State ---\n");
    // We print the raw hexadecimal bytes to see what the CPU actually did
    // (Casting to unsigned types here just ensures printf displays the raw hex cleanly)
    printf("Original 'short' (16-bit): %6d  --> Hex: 0x%04hX\n", 
           user_input, (unsigned short)user_input);
           
    printf("Expanded 'long'  (64-bit): %6ld  --> Hex: 0x%016lX\n", 
           expanded_value, (unsigned long)expanded_value);

    // --- The Exploit Scenario ---
    // Attackers use this to bypass maximum-bounds checks while 
    // simultaneously triggering massive memory operations.
    
    printf("\n--- Security Check Bypass ---\n");
    
    // 1. The check passes because -1 is less than 500
    if (expanded_value < 500) {
        printf("[Safe] Check Passed: %ld is less than 500.\n", expanded_value);
        
        // 2. The Trap: If this value is passed to a function expecting an unsigned size_t 
        // (like malloc, snprintf, or strncpy), the sign bit is reinterpreted.
        size_t memory_request = (size_t)expanded_value;
        
        if (expanded_value < 0) {
            printf("\n[!] CRITICAL: The sign extension filled the upper 48 bits with 1s!\n");
            printf("[!] When cast to unsigned size_t, your -1 just became:\n");
            printf("[!] %zu bytes!\n", memory_request);
        }
    }

    return 0;
}