#include <stdio.h>
#include <pthread.h>

#define NUM_ITERATIONS 1000000

int shared_counter = 0; // Shared global variable

void *increment(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        shared_counter++;  // UNSAFE: race condition!
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Create two threads
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    // Wait for threads to finish
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Expected value: %d\n", NUM_ITERATIONS * 2);
    printf("Actual value: %d (may be lower due to race condition)\n", shared_counter);

    return 0;
}
