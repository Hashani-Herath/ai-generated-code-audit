#include <stdio.h>

#define TARGET_VALUE 100

int main() {
    // 1. The Real-World Data
    // The sensor reads a perfectly safe, low value.
    int sensor_reading = 45;

    printf("--- Assignment in Condition Vulnerability ---\n");
    printf("Initial sensor reading: %d\n", sensor_reading);
    printf("Target threshold: %d\n\n", TARGET_VALUE);

    // 2. THE VULNERABILITY
    // The developer meant to write: if (sensor_reading == TARGET_VALUE)
    // But they missed one equals sign.
    
    printf("Evaluating: if (sensor_reading = TARGET_VALUE)...\n\n");
    
    if (sensor_reading = TARGET_VALUE) {
        // This block will ALWAYS execute, no matter what the sensor actually reads.
        printf("[!] ALARM: Target Reached! Initiating critical shutdown!\n");
    } else {
        printf("[Safe] System operating normally.\n");
    }

    // 3. THE AFTERMATH (Data Corruption)
    printf("\n--- Memory State After 'if' Statement ---\n");
    printf("Current sensor reading is now: %d\n", sensor_reading);

    if (sensor_reading == TARGET_VALUE) {
        printf("\n[!] CRITICAL: The 'if' statement didn't just misread the sensor.\n");
        printf("[!] It actively OVERWROTE the sensor data with the target value!\n");
    }

    // --- THE SECURE APPROACH ("Yoda Conditions") ---
    // If you reverse the order, the compiler catches the typo for you.
    /*
    if (TARGET_VALUE == sensor_reading) { ... } // Works perfectly
    if (TARGET_VALUE = sensor_reading) { ... }  // COMPILER ERROR: Cannot assign to a constant
    */

    return 0;
}