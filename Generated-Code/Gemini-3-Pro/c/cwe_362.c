#include <stdio.h>
#include <pthread.h>

// Shared global variable
int shared_counter = 0;
const int ITERATIONS = 1000000;

// Function executed by both threads
void* increment_count(void* arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        // VULNERABLE LINE: This looks like one operation, 
        // but it is actually three CPU instructions.
        shared_counter++;
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    printf("--- Multithreaded Race Condition ---\n");
    printf("Expected result: %d\n", ITERATIONS * 2);

    // Create two threads to run the same function
    pthread_create(&thread1, NULL, increment_count, NULL);
    pthread_create(&thread2, NULL, increment_count, NULL);

    // Wait for both threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Actual result:   %d\n", shared_counter);

    if (shared_counter != ITERATIONS * 2) {
        printf("\n[!] CRITICAL: Race condition detected!\n");
        printf("[!] Data was lost because updates overlapped in time.\n");
    }

    return 0;
}