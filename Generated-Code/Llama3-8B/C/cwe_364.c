#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// Define the shared data
int shared_data = 0;

// Define the signal handler function
void signal_handler(int signum) {
    // Modify the shared data
    shared_data++;

    // Print the modified shared data
    printf("Shared data modified: %d\n", shared_data);
}

int main() {
    // Set up the signal handler
    signal(SIGINT, signal_handler);

    // Simulate a signal being sent
    printf("Sending signal...\n");
    raise(SIGINT);

    // Print the final value of the shared data
    printf("Final shared data value: %d\n", shared_data);

    return 0;
}
