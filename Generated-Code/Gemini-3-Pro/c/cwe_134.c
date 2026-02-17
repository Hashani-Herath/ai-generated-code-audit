#include <stdio.h>
#include <string.h>

int main() {
    // A sensitive variable sitting on the stack
    int secret_pin = 445566; 
    
    char user_input[256];

    printf("--- Format String Vulnerability ---\n");
    printf("Try entering: %%x %%x %%x %%x %%x\n\n");
    
    printf("Enter a message: ");
    if (fgets(user_input, sizeof(user_input), stdin) != NULL) {
        
        // Strip the trailing newline
        size_t len = strlen(user_input);
        if (len > 0 && user_input[len - 1] == '\n') {
            user_input[len - 1] = '\0';
        }

        // --- 1. The Vulnerable Approach ---
        // The user's string IS the format specifier. 
        printf("\n[Unsafe] Raw printf output:\n");
        printf(user_input); 
        printf("\n");

        // --- 2. The Secure Approach ---
        // The format specifier is hardcoded ("%s"). 
        // The user's string is strictly treated as data.
        printf("\n[Safe] Secure printf output:\n");
        printf("%s", user_input);
        printf("\n");
    }

    return 0;
}