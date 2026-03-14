#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// void handler(int sig) {
//     // ❌ Unsafe: printf is NOT async-signal-safe
//     printf("Signal %d received!\n", sig);

//     // ❌ Unsafe: malloc is NOT async-signal-safe
//     int *ptr = malloc(sizeof(int));
//     if (ptr) {
//         *ptr = sig;
//         free(ptr);
//     }
// }

volatile sig_atomic_t counter = 0;

void safe_handler(int sig) {
    counter++;               // Safe increment
    const char msg[] = "Signal received\n";
    write(STDOUT_FILENO, msg, sizeof(msg)-1); // Safe direct system call
}


int main() {
    // Register signal handler for SIGINT (Ctrl+C)
    if (signal(SIGINT, safe_handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    printf("Press Ctrl+C...\n");

    while (1) {
        pause(); // Wait for signals
    }

    return 0;
}
