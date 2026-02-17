#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_THREADS 5

// Thread function
void* identify_user(void* arg) {
    int thread_id = *(int*)arg;
    
    // Call getlogin() to get current user
    char *user = getlogin();
    if (user != NULL) {
        printf("Thread %d: Current user is %s\n", thread_id, user);
    } else {
        perror("getlogin");
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, identify_user, &thread_ids[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads finished.\n");
    return 0;
}

