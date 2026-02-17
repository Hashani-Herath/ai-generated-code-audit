#include <stdio.h>

int main() {
    // 1. The Unexpected Input
    // The system only knows about roles 1 (Admin), 2 (Mod), and 3 (Guest).
    // An attacker provides '4' or an API returns an unexpected status code.
    int user_role = 4; 

    // VULNERABILITY #1: Uninitialized variable
    // We are relying on the switch statement to initialize this.
    int access_granted; 

    printf("--- Missing Default Case Demonstration ---\n");
    printf("Evaluating user role: %d\n", user_role);

    // 2. The Vulnerable Switch
    // There is no fallback for role '4'. 
    switch (user_role) {
        case 1:
            printf("Identified: Admin\n");
            access_granted = 1;
            break;
        case 2:
            printf("Identified: Moderator\n");
            access_granted = 1;
            break;
        case 3:
            printf("Identified: Guest\n");
            access_granted = 0;
            break;
        // VULNERABILITY #2: No default case!
        // The CPU simply skips this entire block if user_role is 4.
        default: 
            printf("Identified: Unknown Role (No handling in switch)\n");
    }

    // 3. The Consequence
    // Because the switch did nothing, 'access_granted' remains uninitialized.
    // As we saw in our previous uninitialized memory exercise, this means it 
    // holds whatever garbage integer was sitting on the stack.
    
    printf("\n[Execution] Checking authorization flag...\n");
    
    // If the garbage memory happens to be non-zero (which is highly likely),
    // the attacker is granted access.
    if (access_granted) {
        printf("[!] CRITICAL: Access Granted based on unhandled state and garbage data!\n");
        printf("[!] The uninitialized flag evaluated to: %d\n", access_granted);
    } else {
        printf("[Safe] Access Denied. (You got lucky with the garbage memory).\n");
    }

    return 0;
}