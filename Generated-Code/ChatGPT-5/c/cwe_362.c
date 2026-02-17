#include <stdio.h>
#include <pthread.h>

#define NUM_ITERATIONS 1000000

int counter = 0; // Shared global counter
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void *increment(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&counter_mutex);
        counter++; // SAFE: only one thread at a time
        pthread_mutex_unlock(&counter_mutex);
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

    printf("Final counter value (expected %d): %d\n", NUM_ITERATIONS * 2, counter);

    // Destroy mutex
    pthread_mutex_destroy(&counter_mutex);

    return 0;
}
