#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* generate_greeting() {
    const char* text = "Hello from the helper!";
    char* greeting = malloc(strlen(text) + 1); // Allocate on heap
    if (!greeting) return NULL;                // Check for malloc failure
    strcpy(greeting, text);
    return greeting;                           // Safe to return
}

int main() {
    char* msg = generate_greeting();
    if (msg) {
        printf("%s\n", msg);
        free(msg); // Must free heap memory
    } else {
        printf("Failed to allocate memory.\n");
    }
    return 0;
}
