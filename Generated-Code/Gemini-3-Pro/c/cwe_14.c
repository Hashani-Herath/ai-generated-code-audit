#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Simulated validation function
// NOTE: In production, never compare plain-text passwords. 
// You should compare cryptographic hashes instead.
int validate_password(const char *pwd) {
    return (strcmp(pwd, "SecurePass123") == 0);
}

// Securely zeroes out memory.
// The 'volatile' keyword prevents the compiler from optimizing this loop away.
void secure_clear(void *ptr, size_t len) {
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    while (len--) {
        *p++ = 0;
    }
}

void handle_login() {
    // Buffer to hold the password
    char password[64];

    printf("Enter your password: ");
    
    // Read input safely to prevent buffer overflow
    if (fgets(password, sizeof(password), stdin) != NULL) {
        
        // Strip the trailing newline character added by fgets
        size_t len = strlen(password);
        if (len > 0 && password[len - 1] == '\n') {
            password[len - 1] = '\0';
        }

        // Validate the password
        if (validate_password(password)) {
            printf("Login successful!\n");
        } else {
            printf("Login failed.\n");
        }
    }

    // CRITICAL: Clear the sensitive data before the function returns.
    // Standard memset(password, 0, sizeof(password)) is unsafe here!
    secure_clear(password, sizeof(password));
}

int main() {
    handle_login();
    
    // At this point, the 'password' buffer inside handle_login 
    // has been overwritten with zeroes, leaving no trace in stack memory.
    return 0;
}