#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>

#define TARGET_VALUE 100
#define SENSOR_ID "TEMP-02"
#define CHECK_INTERVAL 1  // seconds

volatile bool running = true;

void signal_handler(int sig) {
    printf("\nShutting down sensor monitor...\n");
    running = false;
}

// Simulate sensor reading (random value)
int read_sensor() {
    // Simulate sensor with random values between 0 and 150
    return rand() % 151;
}

int main() {
    int reading;
    int readings_count = 0;
    time_t start_time, current_time;
    
    // Set up signal handler for Ctrl+C
    signal(SIGINT, signal_handler);
    
    // Initialize random seed
    srand(time(NULL));
    
    printf("Continuous Sensor Monitor\n");
    printf("=========================\n");
    printf("Sensor ID: %s\n", SENSOR_ID);
    printf("Target Value: %d\n", TARGET_VALUE);
    printf("Checking every %d second(s)\n", CHECK_INTERVAL);
    printf("Press Ctrl+C to stop\n\n");
    
    start_time = time(NULL);
    
    while (running) {
        reading = read_sensor();
        readings_count++;
        
        current_time = time(NULL);
        printf("[%02ld:%02ld] Reading #%d: %3d - ", 
               (current_time - start_time) / 60,
               (current_time - start_time) % 60,
               readings_count, reading);
        
        if (reading == TARGET_VALUE) {
            printf("🎯 TARGET REACHED!\n");
            
            // Optional: Add special actions when target reached
            printf("*** ALERT: Target value detected! ***\n");
            printf("*** Taking appropriate actions... ***\n");
            
            // Could trigger alarms, log events, etc.
        } else {
            printf("Target not reached\n");
        }
        
        sleep(CHECK_INTERVAL);
    }
    
    printf("\nSensor monitor stopped.\n");
    printf("Total readings taken: %d\n", readings_count);
    
    return 0;
}