#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- THE LIBRARY'S INTERNAL STATE ---
// In a secure library, this struct would be hidden in a .c file.
// Because it is exposed for "debugging", the external user knows its layout.
typedef struct {
    int is_premium_user;
    int _internal_state_machine;
    char *session_token;
} AppEngineState;

// The library's private global instance
AppEngineState core_engine = {0, 1, NULL};

// 1. THE VULNERABLE DEBUG API
// Returning a raw pointer to the internal state.
AppEngineState* get_debug_state() {
    printf("[Library] WARNING: Debug state pointer exposed.\n");
    return &core_engine;
}

// A standard API function that relies on internal integrity
void access_premium_features() {
    if (core_engine.is_premium_user) {
        printf("[Library] Access Granted: Executing Premium Features...\n");
    } else {
        printf("[Library] Access Denied: Free tier limitations apply.\n");
    }
}

// --- THE EXTERNAL DEVELOPER'S CODE ---
int main() {
    printf("--- Encapsulation Bypass Demonstration ---\n\n");

    // Standard behavior: The user is not premium.
    access_prompt();
    access_premium_features();

    // 2. THE EXPLOIT
    // The external developer asks for the debug state.
    AppEngineState *leaked_state = get_debug_state();

    printf("\n[External] Inspecting state... is_premium_user = %d\n", leaked_state->is_premium_user);
    printf("[External] Maliciously modifying internal library state...\n\n");

    // 3. THE CORRUPTION
    // Because they have a raw pointer, they can bypass all setter functions 
    // and payment validations. They just overwrite the memory directly.
    leaked_state->is_premium_user = 1; 

    // They could also corrupt pointers, causing a Use-After-Free in the library!
    // leaked_state->session_token = (char*)0xDEADBEEF; 

    // 4. THE CONSEQUENCE
    // The library now trusts its corrupted internal state.
    access_premium_features();

    return 0;
}

// Helper for the demo output
void access_prompt() { printf("Attempting premium access via standard API...\n"); }