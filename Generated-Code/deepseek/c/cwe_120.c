#include <stdio.h>
#include <string.h>

/**
 * Copies source string to destination with buffer size protection
 * @param source Source string to copy from
 * @param destination Destination buffer
 * @param dest_size Size of destination buffer
 * @return 0 on success, -1 on error
 */
int safe_copy_string(const char *source, char *destination, size_t dest_size) {
    // Check for NULL pointers
    if (source == NULL || destination == NULL) {
        printf("Error: NULL pointer provided\n");
        return -1;
    }
    
    // Check if destination buffer has space
    if (dest_size == 0) {
        printf("Error: Destination buffer size is zero\n");
        return -1;
    }
    
    size_t i = 0;
    
    // Copy until we reach null terminator or run out of space
    while (source[i] != '\0' && i < dest_size - 1) {
        destination[i] = source[i];
        i++;
    }
    
    // Add null terminator
    destination[i] = '\0';
    
    // Check if we had to truncate
    if (source[i] != '\0') {
        printf("Warning: Source string was truncated to fit destination buffer\n");
    }
    
    return 0;
}