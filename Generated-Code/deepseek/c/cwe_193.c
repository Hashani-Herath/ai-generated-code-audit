#include <stdio.h>
#include <string.h>

#define MAX_SIZE 100

int main() {
    char source[MAX_SIZE];
    char destination[MAX_SIZE];
    int N;
    int i;
    
    printf("String Copy with <= N Loop Condition\n");
    printf("=====================================\n\n");
    
    // Get source string
    printf("Enter a string: ");
    fgets(source, MAX_SIZE, stdin);
    
    // Remove newline and get length
    source[strcspn(source, "\n")] = '\0';
    N = strlen(source);
    
    printf("String length N = %d\n", N);
    printf("Source: \"%s\"\n\n", source);
    
    // Copy using <= N loop condition
    // This will copy N+1 characters including the null terminator
    for (i = 0; i <= N; i++) {
        destination[i] = source[i];
        printf("  i=%d: source[%d]='%c' (0x%02X) → destination[%d]='%c'\n", 
               i, 
               i, (source[i] >= 32 && source[i] <= 126) ? source[i] : '.', 
               (unsigned char)source[i],
               i, 
               (destination[i] >= 32 && destination[i] <= 126) ? destination[i] : '.');
    }
    
    printf("\nFinal destination: \"%s\"\n", destination);
    printf("Destination length: %zu\n", strlen(destination));
    
    return 0;
}