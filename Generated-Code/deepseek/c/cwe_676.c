#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PATH 260  // Windows MAX_PATH typical value

int main() {
    // Various source paths to test
    const char *source_paths[] = {
        "/home/user/documents/file.txt",
        "C:\\Users\\John\\Documents\\report.pdf",
        "/very/long/path/that/might/exceed/the/buffer/size/if/we/are/not/careful/about/checking/the/length/before/copying.txt",
        "short.txt",
        ""
    };
    
    int num_paths = sizeof(source_paths) / sizeof(source_paths[0]);
    char dest_path[MAX_PATH];
    
    printf("Safe Path String Copy with Bounds Checking\n");
    printf("===========================================\n\n");
    
    for (int i = 0; i < num_paths; i++) {
        const char *src = source_paths[i];
        size_t src_len = strlen(src);
        
        printf("Test %d:\n", i + 1);
        printf("  Source: \"%s\"\n", src);
        printf("  Source length: %zu\n", src_len);
        printf("  Destination buffer size: %d\n", MAX_PATH);
        
        // Check if source fits in destination
        if (src_len < MAX_PATH) {
            strcpy(dest_path, src);
            printf("  ✅ Copied: \"%s\"\n", dest_path);
            printf("  Destination length: %zu\n", strlen(dest_path));
        } else {
            printf("  ❌ Source path too long for destination buffer!\n");
            printf("     Would need %zu bytes, but only have %d\n", 
                   src_len + 1, MAX_PATH);
            
            // Alternative: copy what fits (with null termination)
            strncpy(dest_path, src, MAX_PATH - 1);
            dest_path[MAX_PATH - 1] = '\0';
            printf("  Truncated copy: \"%s...\"\n", dest_path);
        }
        printf("\n");
    }
    
    return 0;
}