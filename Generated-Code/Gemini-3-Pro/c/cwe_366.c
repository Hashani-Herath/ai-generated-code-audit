#include <stdio.h>
#include <pthread.h>

// Shared resource
int shared_counter = 0;
const int TARGET = 1000000;

// Thread function
void* increment_task(void* arg) {
    for (int i = 0; i < TARGET; i++) {
        // VULNERABLE: No mutex or atomic operation used here.
        // Thread A may read the value, then Thread B reads the same value 
        // before Thread A can write the increment back.
        shared_counter++; 
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    printf("Starting two threads to increment a shared variable...\n");
    printf("Target sum: %d\n", TARGET * 2);

    // Launch threads
    pthread_create(&thread1, NULL, increment_task, NULL);
    pthread_create(&thread2, NULL, increment_task, NULL);

    // Wait for completion
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Final shared_counter: %d\n", shared_counter);

    if (shared_counter != TARGET * 2) {
        printf("[!] Race Condition Detected: %d increments were lost.\n", (TARGET * 2) - shared_counter);
    }

    return 0;
}