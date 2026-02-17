#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void demonstrate_vulnerability() {
    char user_input[200];
    int secret_value = 42;
    
    printf("\n=== Format String Vulnerability Demo ===\n");
    printf("Secret value (for demonstration): %d at address %p\n", 
           secret_value, (void*)&secret_value);
    
    printf("\nEnter a format string to see what happens: ");
    fgets(user_input, sizeof(user_input), stdin);
    
    // Remove newline
    user_input[strcspn(user_input, "\n")] = '\0';
    
    printf("\n--- Using printf directly with user input ---\n");
    printf("Calling: printf(\"%s\")\n\n", user_input);
    
    // DANGEROUS: This can leak stack information
    printf(user_input);
    printf("\n");
    
    printf("\n--- For comparison, safe version ---\n");
    printf("Calling: printf(\"%%s\", \"%s\")\n\n", user_input);
    printf("Safe output: %s\n", user_input);
}

void demonstrate_safe_alternatives() {
    char user_input[100];
    
    printf("\n=== Safe Alternatives ===\n");
    printf("Enter some text: ");
    fgets(user_input, sizeof(user_input), stdin);
    user_input[strcspn(user_input, "\n")] = '\0';
    
    // Safe way 1: Use %s
    printf("\n1. Using %%s: %s\n", user_input);
    
    // Safe way 2: Use puts
    printf("2. Using puts: ");
    puts(user_input);
    
    // Safe way 3: Print character by character
    printf("3. Character by character: ");
    for (int i = 0; user_input[i] != '\0'; i++) {
        putchar(user_input[i]);
    }
    printf("\n");
}

int main() {
    int choice;
    
    printf("Format String Security Demo\n");
    printf("===========================\n");
    printf("WARNING: This demonstrates security vulnerabilities!\n");
    printf("Never use user input directly as a format string in real code.\n\n");
    
    printf("1. Demonstrate vulnerability\n");
    printf("2. Show safe alternatives\n");
    printf("3. Exit\n");
    printf("Choice: ");
    scanf("%d", &choice);
    
    // Clear input buffer
    while (getchar() != '\n');
    
    switch(choice) {
        case 1:
            demonstrate_vulnerability();
            break;
        case 2:
            demonstrate_safe_alternatives();
            break;
        case 3:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid choice!\n");
    }
    
    return 0;
}