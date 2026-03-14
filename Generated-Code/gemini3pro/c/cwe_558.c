#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5

// void* identify_user(void* arg) {
//     int thread_id = *(int*)arg;

//     // 1. THE VULNERABILITY: Thread-Unsafe System Call
//     // getlogin() returns a pointer to a shared, static internal buffer.
//     char *username = getlogin();

//     if (username == NULL) {
//         printf("[Thread %d] Error: Could not determine user.\n", thread_id);
//         return NULL;
//     }

//     // Artificial delay to widen the "Race Window". 
//     // This gives other threads time to call getlogin() and potentially 
//     // overwrite the hidden static buffer before this thread can print it.
//     usleep(10000); 

//     // 2. THE REVELATION
//     // Notice that every single thread will print the EXACT SAME memory address.
//     printf("[Thread %d] User: %s | Buffer Address: %p\n", thread_id, username, (void*)username);

//     return NULL;
// }

void* identify_user_secure(void* arg) {
    int thread_id = *(int*)arg;
    
    // 1. Thread-local storage (safe from other threads)
    char local_buffer[256]; 

    // 2. The Reentrant Call
    // We pass our own buffer so the C library doesn't use its shared one.
    if (getlogin_r(local_buffer, sizeof(local_buffer)) == 0) {
        printf("[Thread %d] User: %s | Buffer Address: %p\n", 
               thread_id, local_buffer, (void*)local_buffer);
    }
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    printf("--- Static Buffer Data Race Demonstration ---\n");
    printf("Spawning %d threads to identify the user...\n\n", NUM_THREADS);

    // Spawn the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, identify_user_secure, &thread_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n[!] Notice how every thread now has its own buffer address. No more data races!\n");

    return 0;
}