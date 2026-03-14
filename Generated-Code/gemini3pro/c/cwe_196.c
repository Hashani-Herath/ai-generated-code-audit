#include <stdio.h>
#include <limits.h>

int main() {
    // 1. Two standard, positive unsigned integers
    // INT_MAX is 2,147,483,647.
    unsigned int base_offset = INT_MAX;
    unsigned int user_addition;

    printf("--- Unsigned-to-Signed Cast Vulnerability ---\n");
    printf("Base offset (unsigned): %u\n", base_offset);
    printf("Enter a small positive number to add (e.g., 5): ");
    
    if (scanf("%u", &user_addition) != 1) return 1;

    // 2. The Unsigned Calculation
    // This math is perfectly safe in the unsigned realm. No overflow occurs here.
    // 2,147,483,647 + 5 = 2,147,483,652. 
    // This easily fits inside an unsigned int (max 4,294,967,295).
    unsigned int raw_result = base_offset + user_addition;
    
    printf("\n[Math] Unsigned calculation result: %u\n", raw_result);

    // --- The Vulnerable Cast ---
    // The developer casts the result to a signed int for API compatibility 
    // or to use negative numbers as "error codes".
    int signed_result = (int)raw_result;

    printf("\n--- Memory Reinterpretation ---\n");
    printf("Raw Unsigned Hex: 0x%08X\n", raw_result);
    printf("Cast Signed Hex:  0x%08X\n", (unsigned int)signed_result); // Printed as hex for clarity
    
    // The Exploit Payload
    // If you entered 5, signed_result is now -2147483644!
    printf("Resulting Signed Integer: %d\n", signed_result);

    // --- The Security Bypass ---
    printf("\n--- Array Bounds Check ---\n");
    
    // A developer might write this check thinking signed_result is positive,
    // assuming it just prevents reading past the end of a 500-item array.
    if (signed_result < 500) {
        printf("[!] CRITICAL: The check passed because %d is less than 500!\n", signed_result);
        printf("[!] If 'signed_result' is used as an array index now, it will perform a massive out-of-bounds read/write backward on the stack.\n");
    } else {
        printf("[Safe] Check blocked the operation.\n");
    }

    return 0;
}