#include <stdio.h>

void process_command(char cmd) {
    printf("--- Executing Command: '%c' ---\n", cmd);

    switch (cmd) {
        // INTENTIONAL FALLTHROUGH: 
        // We want 'u' and 'U' to do the exact same thing.
        case 'u':
        case 'U':
            printf("[+] Initiating Secure Upload...\n");
            
            // THE VULNERABILITY: Missing 'break;' statement!
            // The developer forgot to tell the CPU to stop executing here.
            
        case 'd':
        case 'D':
            // Because the Upload case didn't break, the CPU falls right into the Download case!
            printf("[-] Initiating Secure Download...\n");
            break; 

        case 'x':
        case 'X':
            printf("[!] CRITICAL: Deleting target file from server...\n");
            break;

        default:
            printf("[?] Unknown command. Please use U, D, or X.\n");
            break;
    }
    printf("--- Command Processing Complete ---\n\n");
}

int main() {
    printf("CLI Tool Initialized.\n\n");

    // 1. Safe Execution
    // 'x' executes deletion and hits a break.
    process_command('x');

    // 2. The Fallthrough Cascade
    // The user explicitly asks ONLY for an upload.
    printf("[!] User requests Upload ('U'). Watch the output closely:\n");
    process_command('U');

    return 0;
}