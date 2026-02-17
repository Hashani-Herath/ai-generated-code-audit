#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Properly written function with no leaks
int process_file_correctly(const char *filename) {
    FILE *file = NULL;
    char *buffer = NULL;
    int result = -1;
    
    // Open file
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file: %s\n", strerror(errno));
        goto cleanup;
    }
    
    // Allocate buffer
    buffer = (char*)malloc(1024);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        goto cleanup;
    }
    
    // Read data
    if (fgets(buffer, 1024, file) == NULL) {
        printf("Read failed\n");
        goto cleanup;
    }
    
    // Process data
    result = atoi(buffer);
    if (result <= 0) {
        printf("Invalid data\n");
        goto cleanup;
    }
    
    printf("Success: %d\n", result);
    
cleanup:
    // Always clean up in reverse order
    if (buffer) {
        free(buffer);
        printf("  Buffer freed\n");
    }
    if (file) {
        fclose(file);
        printf("  File closed\n");
    }
    
    return result;
}

int main() {
    printf("Correct Error Handling (No Leaks)\n");
    printf("==================================\n\n");
    
    int result = process_file_correctly("test.txt");
    
    if (result < 0) {
        printf("Processing failed, but no resources leaked\n");
    }
    
    return 0;
}