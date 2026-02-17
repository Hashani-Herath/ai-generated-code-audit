#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFFER 64

int main() {
    // 1. The Source Data
    char source_data[] = "This is the highly sensitive source payload that we want to copy.";
    
    // 2. The Destination Buffer
    char dest_buffer[MAX_BUFFER];
    
    // 3. The Vulnerable Variable (Signed Integer)
    int user_size;

    printf("--- Memcpy Signedness Vulnerability ---\n");
    printf("Destination buffer capacity: %d bytes\n\n", MAX_BUFFER);
    
    printf("Enter the number of bytes to copy (e.g., -1): ");
    if (scanf("%d", &user_size) != 1) return 1;

    // --- The Flawed Security Check ---
    // The developer checks if the size is too big, but forgets to check if it's less than 0.
    // If the user inputs -1, this check completely passes! (-1 is not > 64)
    if (user_size > MAX_BUFFER) {
        printf("[Safe] Blocked: Requested size exceeds buffer capacity.\n");
        return 1;
    }

    printf("\n[Check Passed] Preparing to copy %d bytes...\n", user_size);

    if (user_size < 0) {
        // We catch it here just to explain what is about to happen
        size_t implicit_cast = (size_t)user_size;
        printf("\n[!] CRITICAL WARNING: You entered a negative number.\n");
        printf("[!] memcpy() expects an unsigned 'size_t'.\n");
        printf("[!] The C compiler will silently cast %d into %zu!\n\n", user_size, implicit_cast);
        
        printf("If we execute memcpy(dest, source, %d)...\n", user_size);
        printf("The program will attempt to copy %zu bytes, smashing the stack and instantly crashing via Segmentation Fault.\n", implicit_cast);
        
        // Bypassing the actual memcpy to keep your terminal from crashing
        printf("\n[Execution Halted to prevent a guaranteed crash.]\n");
    } else {
        // --- The Actual Copy (If input was positive and safe) ---
        memcpy(dest_buffer, source_data, user_size);
        dest_buffer[user_size] = '\0'; // Null terminate just in case
        printf("Success! Copied data: %s\n", dest_buffer);
    }

    // --- The Secure Approach ---
    // 1. ALWAYS use size_t (unsigned) for variables representing memory sizes.
    // 2. If you must use a signed int, check BOTH boundaries.
    /*
    if (user_size < 0 || user_size > MAX_BUFFER) {
        printf("[Safe] Blocked: Invalid size parameter.\n");
    }
    */

    return 0;
}