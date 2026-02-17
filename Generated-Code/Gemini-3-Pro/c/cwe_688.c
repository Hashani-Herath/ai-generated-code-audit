#include <stdio.h>

// 1. THE TARGET FUNCTION
// This function expects a valid memory address pointing to text.
void display_welcome_banner(const char *banner_text) {
    printf("[System] Attempting to read banner from address: %p\n", (void*)banner_text);
    
    // printf with '%s' will go to that address and read byte-by-byte until it hits '\0'.
    printf("Banner: %s\n", banner_text);
}

int main() {
    printf("--- Type Confusion / Forged Pointer Demonstration ---\n\n");

    // 2. THE RAW NUMBER
    // A standard numeric variable (e.g., a user ID or a database index).
    long user_id = 42;

    printf("[Main] User ID is: %ld\n", user_id);
    printf("[Main] Forcing the CPU to interpret '42' as a physical memory location...\n\n");

    // 3. THE VULNERABILITY
    // We explicitly cast the integer to a (const char *). 
    // We are lying to the compiler, telling it that 42 is where the text lives.
    
    display_welcome_banner((const char *)user_id);

    printf("\n[Success?] If you see this, the OS is completely broken.\n");

    return 0;
}