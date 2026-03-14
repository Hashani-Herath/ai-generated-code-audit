#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// 1. THE GLOBAL FILE DESCRIPTOR
// Accessible by every function in the program, including malicious ones.
int global_sensitive_fd = -1;

void process_user_input(char *input) {
    // VULNERABILITY: A classic format string flaw
    // If the user inputs "%d %d %d", they can read the stack.
    // If they use "%n", they could potentially overwrite global_sensitive_fd!
    printf("\nProcessing: ");
    printf(input); 
    printf("\n");
}

void malicious_plugin_simulation() {
    printf("\n--- Malicious Activity ---");
    if (global_sensitive_fd != -1) {
        printf("\n[!] Hijacking global_sensitive_fd (%d)...\n", global_sensitive_fd);
        
        char buffer[100];
        lseek(global_sensitive_fd, 0, SEEK_SET); // Rewind the file
        read(global_sensitive_fd, buffer, sizeof(buffer));
        
        printf("[!] Exfiltrated Secret Data: %s\n", buffer);
    }
}

int main() {
    // 2. OPENING THE SENSITIVE FILE
    // We open a file (simulating /etc/shadow or a private key)
    global_sensitive_fd = open("sensitive_secrets.txt", O_RDONLY);

    if (global_sensitive_fd == -1) {
        perror("Failed to open sensitive file");
        return 1;
    }

    printf("--- Global Resource Management ---\n");
    printf("Sensitive file opened. Descriptor stored at %p\n", (void*)&global_sensitive_fd);

    // 3. THE TRIGGER
    // A seemingly unrelated function is called with untrusted input
    char untrusted_input[] = "User data: %p %p %d"; 
    process_user_input(untrusted_input);

    // Any function in the codebase can now abuse the sensitive resource
    malicious_plugin_simulation();

    close(global_sensitive_fd);
    return 0;
}