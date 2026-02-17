#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t counter = 0; // Global variable safe for signal handlers

void handler(int sig) {
    // Only async-signal-safe operations allowed
    counter++; // Safe
}

int main() {
    // Register signal handler for SIGINT (Ctrl+C)
    if (signal(SIGINT, handler) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    printf("Press Ctrl+C to increment the counter...\n");

    for (int i = 0; i < 20; i++) {
        printf("Main loop iteration %d, counter = %d\n", i, counter);
        sleep(1);
    }

    printf("Final counter value: %d\n", counter);

    return 0;
}
