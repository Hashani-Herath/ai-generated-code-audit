#include <stdio.h>
#include <stdbool.h>

// Flag definitions
#define FLAG_READ   0x01
#define FLAG_WRITE  0x02
#define FLAG_EXEC   0x04

int main() {
    int permissions = FLAG_READ | FLAG_WRITE;  // Read and write, but not execute
    
    printf("Bit Masking vs Logical Comparison\n");
    printf("==================================\n\n");
    
    printf("Permissions: %s%s%s\n",
           (permissions & FLAG_READ) ? "R" : "-",
           (permissions & FLAG_WRITE) ? "W" : "-",
           (permissions & FLAG_EXEC) ? "X" : "-");
    
    // CORRECT: Bitwise & for mask checking
    printf("\n--- CORRECT: Bitwise & for mask ---\n");
    if (permissions & FLAG_READ) {
        printf("Has read permission\n");
    }
    
    // BUG: Using & when meaning logical AND
    printf("\n--- BUG: Using & for logical condition ---\n");
    if (permissions & FLAG_READ & FLAG_WRITE) {  // Wrong!
        printf("Has both read and write? (probably not what you meant)\n");
    }
    
    // CORRECT: Using && for logical condition
    printf("\n--- CORRECT: Using && for logical condition ---\n");
    if ((permissions & FLAG_READ) && (permissions & FLAG_WRITE)) {
        printf("Has both read AND write permissions (correct)\n");
    }
    
    // Show the difference
    printf("\n=== Expression Values ===\n");
    printf("permissions & FLAG_READ: %d\n", permissions & FLAG_READ);
    printf("permissions & FLAG_WRITE: %d\n", permissions & FLAG_WRITE);
    printf("permissions & FLAG_READ & FLAG_WRITE: %d\n", 
           permissions & FLAG_READ & FLAG_WRITE);
    printf("(permissions & FLAG_READ) && (permissions & FLAG_WRITE): %d\n",
           (permissions & FLAG_READ) && (permissions & FLAG_WRITE));
    
    return 0;
}