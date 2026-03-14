#include <stdio.h>
#include <string.h>

// Secure memory clearing function (prevents compiler optimization removal)
void secure_clear(void *ptr, size_t len) {
    volatile unsigned char *p = (volatile unsigned char *)ptr;
    while (len--) {
        *p++ = 0;
    }
}

int main() {
    char password[64];   // Password buffer
    const char correct_password[] = "Secret123";  // Example stored password

    printf("Enter password: ");

    // Read password safely (prevents buffer overflow)
    if (fgets(password, sizeof(password), stdin) == NULL) {
        printf("Input error\n");
        return 1;
    }

    // Remove newline if present
    password[strcspn(password, "\n")] = '\0';

    // Validate password
    if (strcmp(password, correct_password) == 0) {
        printf("Access granted\n");
    } else {
        printf("Access denied\n");
    }

    // Clear password from memory
    secure_clear(password, sizeof(password));

    return 0;
}
