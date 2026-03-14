#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// We pass the boundaries (max_bytes) and use pointers to return state securely.
char* read_file_secure(const char* filepath, size_t max_bytes, size_t* out_bytes_read) {
    // 1. Paranoid Validation
    if (filepath == NULL || out_bytes_read == NULL || max_bytes == 0) {
        return NULL;
    }

    // Initialize state
    FILE* fp = NULL;
    char* buffer = NULL;
    *out_bytes_read = 0;

    // 2. Open Resource
    fp = fopen(filepath, "rb");
    if (fp == NULL) {
        goto cleanup; // Exit safely if the file doesn't exist
    }

    // 3. Symmetric Ownership (Allocation)
    // We use calloc to avoid "Uninitialized Memory" ghosts.
    buffer = (char*)calloc(max_bytes + 1, sizeof(char)); 
    if (buffer == NULL) {
        goto cleanup; // Exit safely if out of RAM
    }

    // 4. Bounded Operation
    // We only read up to the maximum allowed bytes, preventing overflows.
    *out_bytes_read = fread(buffer, sizeof(char), max_bytes, fp);

    // 5. Single-Point Exit (The Cleanup Gate)
cleanup:
    // If we opened the file, we ALWAYS close it here. No leaks.
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }

    // If something failed and we read 0 bytes, free the buffer so the caller doesn't leak it.
    if (*out_bytes_read == 0 && buffer != NULL) {
        free(buffer);
        buffer = NULL; // The Nullify Habit
    }

    return buffer; // Caller now owns this memory, or receives NULL on failure.
}