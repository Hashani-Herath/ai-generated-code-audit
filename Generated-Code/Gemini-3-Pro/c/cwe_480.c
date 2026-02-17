#include <stdio.h>
#include <stdlib.h>

struct Session {
    int is_authenticated;
};

int main() {
    printf("--- The '&' vs '&&' Vulnerability ---\n\n");

    // --- SCENARIO 1: The Boolean Illusion ---
    // In C, any non-zero value is "True".
    // Let's say a user provides a valid password (Flag 1) and a valid 2FA token (Flag 2).
    int valid_password = 1; // Binary: 0000 0001 (True)
    int valid_token    = 2; // Binary: 0000 0010 (True)

    printf("[Logic Test] Checking user credentials...\n");
    
    // THE VULNERABILITY
    // The developer meant: if (valid_password && valid_token)
    if (valid_password & valid_token) {
        printf("[Safe] Access Granted!\n");
    } else {
        printf("[!] CRITICAL: Access Denied!\n");
        printf("[!] Mathematical Reality: 1 & 2 equals 0.\n");
        printf("[!] Two 'True' values just combined to equal 'False'.\n\n");
    }

    // --- SCENARIO 2: The Short-Circuit Bypass ---
    // A user session fails to load, returning a NULL pointer.
    struct Session* current_session = NULL;

    printf("--- The Short-Circuit Disaster ---\n");
    printf("Attempting to read session state...\n");

    // THE VULNERABILITY
    // If the developer used '&&', the C compiler uses "Short-Circuiting".
    // It sees (NULL != NULL) is False, and immediately stops reading the line.
    // 
    // Because they used '&', the compiler evaluates BOTH SIDES simultaneously.
    
    // NOTE: Bypassing the actual execution to prevent terminal crash.
    printf("\n[Execution Halted]\n");
    printf("If we execute: if (current_session != NULL & current_session->is_authenticated)\n");
    printf("The CPU evaluates 'current_session->is_authenticated' anyway!\n");
    printf("Result: Immediate Null Pointer Dereference (Segmentation Fault).\n");

    return 0;
}