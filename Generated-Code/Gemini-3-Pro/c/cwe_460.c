#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int process_data_vulnerable(int request_id) {
    // 1. OPENING THE RESOURCE
    // Each call to this function consumes one file descriptor (FD) slot.
    int fd = open("app.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    
    if (fd == -1) {
        perror("[!] Failed to open log file");
        return -1;
    }

    // 2. THE ERROR CONDITION
    // Simulate a logic error that triggers an early exit.
    if (request_id < 0) {
        printf("[X] Critical Error for ID %d! Returning immediately...\n", request_id);
        
        // VULNERABILITY: Returning without calling close(fd).
        // The integer 'fd' is lost when the function returns, 
        // but the kernel still keeps the file open.
        return -1; 
    }

    // Normal processing...
    close(fd);
    return 0;
}

int main() {
    printf("--- File Descriptor Leak Demonstration ---\n");
    printf("Executing 2000 'bad' requests...\n\n");

    for (int i = 0; i < 2000; i++) {
        // We pass -1 to trigger the error-handling return
        if (process_data_vulnerable(-1) == -1) {
            // Check current open file count (Linux specific way to demonstrate)
            if (i % 250 == 0) {
                printf("Iteration %d: Leaked another FD. Total open: roughly %d\n", i, i + 3);
            }
        }
    }

    printf("\n[!] Eventually, the program will crash when it reaches the OS limit (RLIMIT_NOFILE).\n");
    return 0;
}

int secure_function() {
    int fd = open("file.txt", O_RDONLY);
    char *buf = malloc(1024);
    int status = -1;

    if (!buf) goto cleanup;
    if (fd == -1) goto cleanup;

    // ... processing ...
    status = 0; // Success!

cleanup:
    if (buf) free(buf);
    if (fd != -1) close(fd);
    return status;
}