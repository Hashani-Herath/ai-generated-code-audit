#include <stdio.h>

int main() {
    // 1. Adjacent sensitive data placed on the stack
    // We put our sentinel character '$' at the end of this secret string.
    char secret_data[] = "TOP_SECRET_API_KEY_8899$";

    // 2. The intended buffer
    // Notice this buffer is only 4 bytes long and contains NO sentinel character.
    char public_buffer[4] = {'U', 'S', 'E', 'R'};

    printf("--- Unbounded Sentinel Search ---\n");
    printf("Reading memory until '$' is found...\n\n");

    int i = 0;
    
    // 3. The Dangerous Loop
    // It has no idea public_buffer is only 4 bytes. It just keeps reading.
    while (1) {
        char current_char = public_buffer[i];
        
        // Print the memory we are currently reading
        printf("public_buffer[%2d] = '%c' (Hex: 0x%02X)\n", 
               i, 
               (current_char >= 32 && current_char <= 126) ? current_char : '.', 
               (unsigned char)current_char);

        // Check for the sentinel character
        if (current_char == '$') {
            printf("\n[!] Sentinel '$' found at index %d! Loop stopped.\n", i);
            break;
        }

        // Safety limit to prevent the program from crashing during this demonstration.
        // In the wild, this loop would run until it hit unmapped memory and Segfaulted.
        if (i >= 40) {
            printf("\n[!] Safety stop triggered to prevent a Segmentation Fault.\n");
            break;
        }

        i++;
    }

    return 0;
}