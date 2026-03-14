#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct User {
    char name[32];
    void (*privilege_check)(); // Function pointer
};

void standard_access() {
    printf("Access Level: [STANDARD USER]\n");
}

void secret_admin_access() {
    printf("Access Level: [SYSTEM ADMINISTRATOR - ROOT]\n");
}

int main() {
    // 1. ALLOCATION
    struct User *current_user = (struct User *)malloc(sizeof(struct User));
    strcpy(current_user->name, "Alice");
    current_user->privilege_check = standard_access;

    printf("--- Use-After-Free Vulnerability ---\n");
    printf("User '%s' allocated at %p\n", current_user->name, (void*)current_user);
    current_user->privilege_check();

    // 2. THE FREE
    printf("\n[!] Logging out user... freeing memory.\n");
    free(current_user);

    /* VULNERABILITY: 'current_user' is now a dangling pointer. 
       The memory is "free," but the pointer still knows where it is.
    */

    // 3. THE RE-ALLOCATION (The Attacker's Move)
    // If an attacker can trigger a new allocation of the same size, 
    // the heap manager will likely give them the EXACT SAME memory block.
    printf("[!] Attacker triggers a malicious allocation...\n");
    long *attacker_data = (long *)malloc(sizeof(struct User));
    
    // The attacker writes a memory address into the first few bytes.
    // They point it to the 'secret_admin_access' function.
    *attacker_data = (long)secret_admin_access; 

    // 4. THE USE-AFTER-FREE
    // The program still thinks 'current_user' is valid and tries to use it.
    printf("\n--- Executing Delayed Process ---\n");
    printf("Attempting to check privileges for old user pointer...\n");
    
    // This call now executes whatever the attacker put in that memory!
    current_user->privilege_check(); 

    return 0;
}