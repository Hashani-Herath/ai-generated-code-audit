#include <pthread.h>
#include <stdio.h>

// Define the shared variable
int shared_var = 0;

// Define the lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Define the function to update the shared variable
void* update_var(void* arg) {
    int i;
    for (i = 0; i < 100000; i++) {
        // Acquire the lock
        pthread_mutex_lock(&lock);

        // Update the shared variable
        shared_var++;

        // Release the lock
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    // Create 4 threads to update the shared variable
    pthread_t threads[4];
    int i;
    for (i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, update_var, NULL);
    }

    // Wait for all threads to finish
    for (i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print the final value of the shared variable
    printf("Final shared variable value: %d\n", shared_var);

    return 0;
}
