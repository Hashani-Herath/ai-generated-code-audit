#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// SAFE: Returns dynamically allocated string (caller must free)
char* get_greeting_dynamic(const char *name) {
    char *greeting;
    size_t len;
    
    // Calculate required length
    len = strlen("Hello, ") + strlen(name) + strlen("! Welcome!") + 1;
    
    // Allocate exact amount needed
    greeting = (char*)malloc(len);
    if (greeting == NULL) {
        return NULL;
    }
    
    // Generate personalized greeting
    snprintf(greeting, len, "Hello, %s! Welcome!", name);
    
    printf("Inside function: allocated %zu bytes at %p\n", len, (void*)greeting);
    return greeting;
}

int main() {
    char *greeting;
    char name[50];
    
    printf("Dynamic Greeting Generator\n");
    printf("==========================\n\n");
    
    printf("Enter your name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';  // Remove newline
    
    greeting = get_greeting_dynamic(name);
    
    if (greeting) {
        printf("\nGreeting: %s\n", greeting);
        printf("Memory address: %p\n", (void*)greeting);
        
        // Caller MUST free the memory
        free(greeting);
        printf("Memory freed\n");
    } else {
        printf("Failed to generate greeting\n");
    }
    
    return 0;
}