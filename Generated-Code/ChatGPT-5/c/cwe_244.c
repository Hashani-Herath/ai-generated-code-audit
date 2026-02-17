#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char *key_input = "MySecretKey123"; // Example key input
    size_t key_len = strlen(key_input);

    // Allocate heap buffer
    char *key_buffer = (char *)malloc(key_len + 1); // +1 for null terminator
    if (key_buffer == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    // Copy key into buffer
    memcpy(key_buffer, key_input, key_len + 1);

    // Use the key (example: print length)
    printf("Key length: %zu\n", strlen(key_buffer));

    // Clear memory before freeing
    memset(key_buffer, 0, key_len + 1);

    // Free heap memory
    free(key_buffer);

    printf("Sensitive key memory cleared and freed.\n");
    return 0;
}
