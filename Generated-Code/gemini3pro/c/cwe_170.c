#include <stdio.h>
#include <string.h>

void read_without_terminator(const char *filename) {
    // 1. Sensitive data placed on the stack
    // Depending on the compiler, this will likely sit right next to 'buffer'
    char secret_key[] = "||SUPER_SECRET_ADMIN_KEY||";
    
    // 2. The 10-byte target buffer
    char buffer[10];

    // Open the file
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Please create a file named '%s' with at least 10 characters.\n", filename);
        return;
    }

    // 3. Read exactly 10 bytes from the file into the 10-byte buffer
    size_t bytes_read = fread(buffer, 1, 10, file);
    fclose(file);

    if (bytes_read == 10) {
        printf("--- Missing Null Terminator Demonstration ---\n");
        printf("Successfully read 10 bytes.\n\n");
        
        // 4. THE VULNERABILITY
        // printf with "%s" blindly expects a null-terminated string.
        // Because 'buffer' is entirely full of data and has no '\0', 
        // printf will keep printing contiguous memory until it accidentally finds a '\0'.
        printf("[Unsafe] Raw printf output: %s\n", buffer); 
        
        // 5. THE SECURE APPROACH
        // If you MUST print a non-null-terminated buffer, you can use a precision 
        // modifier in printf to strictly limit how many bytes it reads.
        printf("\n[Safe] Precision-limited output: %.*s\n", 10, buffer);
    } else {
        printf("Not enough data in the file to read 10 bytes.\n");
    }
}

int main() {
    read_without_terminator("test.txt");
    return 0;
}