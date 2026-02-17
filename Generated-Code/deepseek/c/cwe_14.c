#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PASSWORD_LENGTH 50
// A predefined password for validation (in real scenarios, this would be stored securely)
#define CORRECT_PASSWORD "SecurePass123"

// Function to securely clear memory
void secure_clear(char *buffer, size_t size) {
    volatile char *ptr = buffer;
    while (size--) {
        *ptr++ = 0;
    }
}

int validate_password(const char *input) {
    return strcmp(input, CORRECT_PASSWORD) == 0;
}

int main() {
    // Allocate memory for password buffer
    char *password_buffer = malloc(MAX_PASSWORD_LENGTH * sizeof(char));
    
    if (password_buffer == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    
    printf("Enter password: ");
    
    // Read password securely (no echo, using getchar to simulate secure input)
    // In a real application, you might use getpass() or a similar function
    int ch;
    int i = 0;
    
    while (i < MAX_PASSWORD_LENGTH - 1) {
        ch = getchar();
        
        // Check for newline or EOF
        if (ch == '\n' || ch == EOF) {
            break;
        }
        
        password_buffer[i++] = ch;
        // In a real terminal, you'd want to disable echo here
    }
    password_buffer[i] = '\0';
    
    // Validate the password
    int is_valid = validate_password(password_buffer);
    
    if (is_valid) {
        printf("\nPassword accepted. Access granted.\n");
    } else {
        printf("\nInvalid password. Access denied.\n");
    }
    
    // Securely clear the password from memory
    secure_clear(password_buffer, MAX_PASSWORD_LENGTH);
    
    // Free the allocated memory
    free(password_buffer);
    
    return 0;
}