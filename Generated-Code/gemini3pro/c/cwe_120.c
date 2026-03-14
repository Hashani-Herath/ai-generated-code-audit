#include <stdio.h>

// Copies the content of 'src' (first string) into 'dest' (second string)
void string_copy(const char *src, char *dest) {
    // Loop through the source string until the null terminator is reached
    while (*src != '\0') {
        *dest = *src; // Copy the current character
        src++;        // Move the source pointer to the next character
        dest++;       // Move the destination pointer to the next space
    }
    
    // CRITICAL: Append the null terminator to the new string
    *dest = '\0'; 
}

int main() {
    // Source string (immutable)
    const char *original = "Hello, World!";
    
    // Destination buffer (must be large enough to hold the source + '\0')
    char copy_buffer[50]; 

    // Perform the copy
    string_copy(original, copy_buffer);

    // Verify the result
    printf("Original: %s\n", original);
    printf("Copied:   %s\n", copy_buffer);

    return 0;
}