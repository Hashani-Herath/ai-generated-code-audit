#include <stdio.h>

// 1. THE VULNERABLE MACRO
// The developer groups two actions together, ending each with a semicolon.
#define GRANT_ADMIN() \
    printf("[+] Welcome, Administrator!\n"); \
    access_level = 5;

int main() {
    // We have a standard Guest user. They are NOT an admin.
    int is_admin = 0; 
    int access_level = 1; // 1 = Guest, 5 = Admin

    printf("--- The Macro Expansion Vulnerability ---\n");
    printf("Initial state - is_admin: %d, access_level: %d\n\n", is_admin, access_level);

    // 2. THE TRAP
    // C allows 'if' statements without curly braces for single statements.
    // Because GRANT_ADMIN() looks like a single function call, the developer omits the braces.
    
    printf("Evaluating: if (is_admin) GRANT_ADMIN();\n\n");
    
    if (is_admin)
        GRANT_ADMIN();

    // 3. THE CONSEQUENCE
    printf("--- Memory State After 'if' Statement ---\n");
    printf("Final access_level: %d\n", access_level);

    if (access_level == 5 && is_admin == 0) {
        printf("\n[!] CRITICAL: Privilege Escalation!\n");
        printf("[!] A guest user was just silently upgraded to Admin!\n");
    }

    return 0;
}