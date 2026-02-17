#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256

// Global data structure
typedef struct {
    char message[BUFFER_SIZE];
    int counter;
    int flag;
} SharedData;

SharedData global_data = {
    .message = "Initial message",
    .counter = 0,
    .flag = 0
};

// Flag to control program execution
volatile int keep_running = 1;

// Signal handler for SIGINT (Ctrl+C)
void signal_handler(int sig) {
    printf("\n\nSignal %d caught in handler!\n", sig);
    
    // Modify global data structure
    strcpy(global_data.message, "Modified by signal handler");
    global_data.counter += 100;
    global_data.flag = 1;
    
    printf("Handler modified global data:\n");
    printf("  Message: %s\n", global_data.message);
    printf("  Counter: %d\n", global_data.counter);
    printf("  Flag: %d\n\n", global_data.flag);
    
    // Don't exit immediately
    keep_running = 0;
}

int main() {
    struct sigaction sa;
    
    // Set up signal handler
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    
    printf("Signal Handler Race Condition Demo\n");
    printf("===================================\n");
    printf("Press Ctrl+C to trigger signal handler\n");
    printf("Main program will keep modifying data\n\n");
    
    int local_counter = 0;
    
    while (keep_running) {
        // Main program modifies global data
        snprintf(global_data.message, BUFFER_SIZE, 
                 "Main iteration %d", local_counter);
        global_data.counter = local_counter;
        global_data.flag = local_counter % 2;
        
        // Print current state occasionally
        if (local_counter % 1000000 == 0) {
            printf("Main - Counter: %d, Message: %s, Flag: %d\n",
                   global_data.counter,
                   global_data.message,
                   global_data.flag);
        }
        
        local_counter++;
    }
    
    printf("\nProgram exiting...\n");
    printf("Final global data:\n");
    printf("  Message: %s\n", global_data.message);
    printf("  Counter: %d\n", global_data.counter);
    printf("  Flag: %d\n", global_data.flag);
    
    return 0;
}