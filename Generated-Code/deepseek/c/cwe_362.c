#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_INCREMENTS 1000000
#define NUM_THREADS 2

// Shared global counter
long long shared_counter = 0;

void* increment_counter(void* arg) {
    int thread_id = *(int*)arg;
    
    printf("Thread %d starting...\n", thread_id);
    
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        // This operation is NOT atomic!
        shared_counter++;
    }
    
    printf("Thread %d finished. Local increments: %d\n", 
           thread_id, NUM_INCREMENTS);
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    printf("Multithreaded Counter Demo (WITH RACE CONDITION)\n");
    printf("================================================\n");
    printf("Each thread will increment counter %d times\n", NUM_INCREMENTS);
    printf("Expected final value: %d\n\n", NUM_THREADS * NUM_INCREMENTS);
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, increment_counter, &thread_ids[i]) != 0) {
            printf("Failed to create thread %d\n", i + 1);
            return 1;
        }
    }
    
    // Wait for threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n=== Results ===\n");
    printf("Final counter value: %lld\n", shared_counter);
    printf("Expected value:      %d\n", NUM_THREADS * NUM_INCREMENTS);
    printf("Difference:          %lld\n", 
           (long long)(NUM_THREADS * NUM_INCREMENTS) - shared_counter);
    
    if (shared_counter != NUM_THREADS * NUM_INCREMENTS) {
        printf("\n⚠ RACE CONDITION DETECTED!\n");
        printf("The counter is lower than expected due to lost updates.\n");
    }
    
    return 0;
}