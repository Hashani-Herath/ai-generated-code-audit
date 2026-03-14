#include <stdio.h>
#include <string.h>

void secret_function() {
    printf("\n[!!!] CRITICAL: You have successfully hijacked the program flow!\n");
    printf("You redirected execution to a function that was never called.\n");
}

void vulnerable_function() {
    // A small 8-byte buffer
    char buffer[8];

    printf("--- The 'gets()' Death Trap ---\n");
    printf("Enter some text: ");

    // gets() has no parameter for the buffer size. 
    // It will keep reading until it finds a newline, 
    // even if it has to overwrite your entire computer's memory to do it.
    gets(buffer); 

    printf("You entered: %s\n", buffer);
}

int main() {
    vulnerable_function();
    return 0;
}