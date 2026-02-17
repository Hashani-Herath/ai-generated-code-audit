#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// --- THE VULNERABLE HANDLER ---
// This function can interrupt the main thread at ANY microsecond.
void handle_interrupt(int sig) {
    // VULNERABILITY 1: printf is not async-signal-safe.
    // It uses internal locks to manage the stdout stream.
    printf("\n[Signal] Interrupted by user! Preparing shutdown...\n");

    // VULNERABILITY 2: malloc is not async-signal-safe.
    // It uses internal locks to manage the heap's free list.
    char *shutdown_msg = (char *)malloc(256);
    
    if (shutdown_msg) {
        strcpy(shutdown_msg, "[Signal] Shutdown memory allocated safely.\n");
        // VULNERABILITY 3: free is not async-signal-safe.
        printf("%s", shutdown_msg);
        free(shutdown_msg);
    }

    // exit() is also unsafe! (_exit() is the safe version)
    exit(1); 
}

int main() {
    // Register the unsafe handler
    signal(SIGINT, safe_interrupt);

    printf("--- Async-Signal Deadlock Demonstration ---\n");
    printf("The main thread is heavily using the heap...\n");
    printf("Press Ctrl+C repeatedly. Eventually, it will completely freeze (Deadlock).\n\n");

    while (1) {
        // Main program doing heavy heap operations
        // malloc() acquires the global "Heap Lock" internally
        void *data = malloc(1024 * 1024); 
        
        // If the SIGINT arrives exactly here, the main thread is suspended 
        // WHILE HOLDING the heap lock!
        
        if (data) {
            free(data); // free() also requires the global Heap Lock
        }
    }

    return 0;
}

// SECURE HANDLER
void safe_interrupt(int sig) {
    // write() is a raw system call and is guaranteed to be async-signal-safe.
    const char msg[] = "\n[Signal] Exiting safely.\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    
    // _exit() immediately asks the kernel to terminate, bypassing standard library cleanup
    _exit(1); 
}