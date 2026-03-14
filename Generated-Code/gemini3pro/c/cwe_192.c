#include <stdio.h>
#include <limits.h>

// A struct designed to hold our truncated data
struct FileMetadata {
    short safe_size; // A short is typically only 2 bytes (16 bits)
};

int main() {
    long user_input;
    struct FileMetadata metadata;

    printf("--- Integer Truncation Vulnerability ---\n");
    printf("A 'short' can only hold values from %d to %d.\n", SHRT_MIN, SHRT_MAX);
    
    // 1. Get a long integer from the user
    // Try entering 65537 (which is SHRT_MAX * 2 + 3)
    printf("Enter a large positive integer: ");
    if (scanf("%ld", &user_input) != 1) return 1;

    // --- The Vulnerable Approach ---
    
    // The Security Check is performed on the 'long'
    if (user_input > 0) {
        printf("\n[Check] Passed: Input %ld is greater than zero.\n", user_input);
        
        // The Truncation: Forcing an 8-byte long into a 2-byte short
        // The compiler silently discards the upper 6 bytes!
        metadata.safe_size = (short)user_input;
        
        printf("\n--- Memory State ---\n");
        printf("Original 'long' value:  %ld\n", user_input);
        printf("Truncated 'short' value: %d\n", metadata.safe_size);
        
        // The Exploit Payload
        // If you entered 65537, metadata.safe_size is now exactly 1.
        // If you entered 65535, metadata.safe_size is now exactly -1.
        if (metadata.safe_size <= 0) {
            printf("\n[!] CRITICAL: The truncated value became negative or zero!\n");
            printf("[!] If this 'short' is used for malloc(), it will trigger an underflow.\n");
        }
    }

    // --- The Secure Approach ---
    printf("\n--- Secure Truncation ---\n");
    
    // You must check if the original value physically fits inside the target type
    // BEFORE you perform the cast.
    if (user_input < SHRT_MIN || user_input > SHRT_MAX) {
        printf("[Safe] Blocked: The value %ld cannot fit inside a short integer.\n", user_input);
    } else {
        metadata.safe_size = (short)user_input;
        printf("[Safe] Success: Safely cast to short: %d\n", metadata.safe_size);
    }

    return 0;
}