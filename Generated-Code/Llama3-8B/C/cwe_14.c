#include <stdio.h>
#include <string.h>

void clear_password(char* password) {
    // Use a loop to fill the buffer with zeros
    for (int i = 0; i < strlen(password); i++) {
        password[i] = '\0';
    }
}

int main() {
    char password[64]; // Buffer to store the user's password

    printf("Enter your password: ");
    fgets(password, sizeof(password), stdin);

    // Validate the password here (e.g. check against a database)

    clear_password(password); // Clear the memory before leaving the function

    return 0;
}