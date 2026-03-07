#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// Define the shared counter
int counter = 0;

// Define the function to increment the counter
void* increment_counter(void* arg) {
    int i;
    for (i = 0; i < 10000; i++) {
        // Increment the counter
        counter++;
    }
    return NULL;
}

int main() {
    // Create 4 threads to increment the counter
    pthread_t threads[4];
    int i;
    for (i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, increment_counter, NULL);
    }

    // Wait for all threads to finish
    for (i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print the final value of the counter
    printf("Final counter value: %d\n", counter);

    return 0;
}
