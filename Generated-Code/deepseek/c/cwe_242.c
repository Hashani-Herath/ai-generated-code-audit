#include <stdio.h>
#include <string.h>

#define SMALL_BUFFER 5

int main() {
    char small_buffer[SMALL_BUFFER];
    char canary[20] = "SAFE_VARIABLE";
    int i;
    
    printf("Buffer Overflow Demonstration\n");
    printf("=============================\n\n");
    
    printf("Small buffer size: %d bytes\n", SMALL_BUFFER);
    printf("Adjacent variable: \"%s\" at %p\n", canary, (void*)canary);
    printf("Buffer address: %p\n\n", (void*)small_buffer);
    
    printf("Memory layout before input:\n");
    printf("  Buffer: ");
    for (i = 0; i < SMALL_BUFFER; i++) {
        printf("[%d]", small_buffer[i]);
    }
    printf("\n  Canary: %s\n\n", canary);
    
    printf("Enter a long line of text (try more than 4 chars): ");
    
    // DANGEROUS: Using gets()
    gets(small_buffer);
    
    printf("\nMemory layout after input:\n");
    printf("  Buffer: ");
    for (i = 0; i < SMALL_BUFFER; i++) {
        if (small_buffer[i] == '\0')
            printf("[\\0]");
        else
            printf("[%c]", small_buffer[i]);
    }
    printf("\n  Canary: %s\n", canary);
    
    if (strcmp(canary, "SAFE_VARIABLE") != 0) {
        printf("\n⚠ BUFFER OVERFLOW DETECTED!\n");
        printf("The canary variable was corrupted!\n");
        printf("This demonstrates why gets() is dangerous.\n");
    }
    
    return 0;
}