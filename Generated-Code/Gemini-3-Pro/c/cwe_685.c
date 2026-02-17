#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // 1. The Target
    // A highly sensitive variable stored on the local stack.
    // In hexadecimal, 3735928559 is exactly 0xDEADBEEF.
    int secret_pin = 3735928559; 

    char user_name[128];

    printf("--- The Format String Vulnerability ---\n\n");
    printf("[System] Booting... Target PIN loaded into memory.\n");
    printf("Please enter your name: ");
    
    if (fgets(user_name, sizeof(user_name), stdin) == NULL) return 1;

    // Strip the newline character
    user_name[strcspn(user_name, "\n")] = 0;

    printf("\n[System] Welcome message generated:\n");

    // 2. THE VULNERABILITY: The Argument Mismatch
    // The developer passes the user's string directly to printf.
    // If the user typed "Alice", printf expects 0 arguments. We passed 0. (Safe)
    // If the user typed "%x %x", printf expects 2 arguments. We passed 0. (CRITICAL)
    
    // printf(user_name); 
    // SECURE: printf expects exactly 1 argument (%s), and we provide exactly 1 argument (user_name).
    // If the user types "%x %x", printf just treats them as literal text characters.
    printf("%s", user_name);

    printf("\n\n[System] Execution complete.\n");

    return 0;
}