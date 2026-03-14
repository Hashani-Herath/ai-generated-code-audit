#include <stdio.h>
#include <string.h>

// We group these to guarantee they sit next to each other in memory
typedef struct {
    char dest_path[16];  // Can only hold 15 characters + 1 null terminator
    int is_admin;        // 0 = False, 1 = True
} SessionState;

int main() {
    SessionState current_session;
    current_session.is_admin = 0; // The user is a standard guest

    printf("--- The Classic Buffer Overflow (strcpy) ---\n\n");
    printf("[Init] Destination Buffer Capacity: %zu bytes\n", sizeof(current_session.dest_path));
    printf("[Init] Admin Status: %d\n\n", current_session.is_admin);

    // 1. The Malicious Input
    // This path is 42 characters long. It vastly exceeds the 16-byte limit.
    const char *user_provided_path = "/var/www/uploads/malicious_user_script.sh";

    // 2. THE VULNERABILITY
    // strcpy NEVER checks the size of the destination. 
    // It will blindly write all 42 characters, spilling into adjacent memory.
    printf("[System] Executing: strcpy(dest_path, user_provided_path)...\n\n");
    
    strcpy(current_session.dest_path, user_provided_path);

    // 3. THE CONSEQUENCE
    printf("--- Memory State After Overflow ---\n");
    printf("Copied Path: %s\n", current_session.dest_path);
    printf("Admin Status: %d\n", current_session.is_admin);

    // Because the letter 'm' from "malicious" spilled over into the is_admin integer,
    // its ASCII value (109) overwrote the 0. The system now sees is_admin as True!
    if (current_session.is_admin != 0) {
        printf("\n[!] CRITICAL: Privilege Escalation!\n");
        printf("[!] The string overflowed and corrupted the admin flag!\n");
    }

    return 0;
}