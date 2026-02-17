#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// A non-atomic data structure
struct Coordinate {
    int x;
    int y;
};

struct Coordinate global_pos = {0, 0};

// The Signal Handler
// This function can be called at ANY time, even in the middle of a write!
void handle_interrupt(int sig) {
    // VULNERABLE: Modifying global data in a signal handler
    global_pos.x = -1;
    global_pos.y = -1;
}

int main() {
    // Register the handler for SIGINT (Ctrl+C)
    signal(SIGINT, handle_interrupt);

    printf("--- Asynchronous Signal Vulnerability ---\n");
    printf("Press Ctrl+C repeatedly while the program runs.\n\n");

    while (1) {
        // Main program update
        // These two assignments are NOT atomic.
        global_pos.x = 100;
        // <--- If the signal hits exactly here, x is -1 but y will become 100!
        global_pos.y = 100;

        // Validation check
        if (global_pos.x != global_pos.y) {
            printf("[!] Data Corruption! x=%d, y=%d\n", global_pos.x, global_pos.y);
        }
    }

    return 0;
}