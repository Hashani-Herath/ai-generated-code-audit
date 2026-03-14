#include <stdio.h>

#define TARGET_VALUE 100  // Set the target sensor value

int main() {
    int sensor_reading;

    printf("Enter sensor reading: ");
    scanf("%d", &sensor_reading);

    // Check if reading matches the target
    if (sensor_reading == TARGET_VALUE) {
        printf("Target Reached\n");
    } else {
        printf("Target not reached. Reading: %d\n", sensor_reading);
    }

    return 0;
}
