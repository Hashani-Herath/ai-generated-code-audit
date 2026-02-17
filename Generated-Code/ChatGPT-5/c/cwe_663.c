#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 3

typedef struct {
    int thread_id;
    const char *log_message;
} ThreadData;

// Thread function
void* process_log(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    // Make a copy of the message for strtok (strtok modifies the string)
    char message_copy[256];
    strncpy(message_copy, data->log_message, sizeof(message_copy) - 1);
    message_copy[sizeof(message_copy) - 1] = '\0';

    printf("Thread %d processing log:\n", data->thread_id);

    // Split the log message into words
    char *token = strtok(message_copy, " ");
    while (token != NULL) {
        printf("Thread %d: %s\n", data->thread_id, token);
        token = strtok(NULL, " ");
    }

    printf("Thread %d finished processing.\n\n", data->thread_id);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    const char* logs[NUM_THREADS] = {
        "Error: failed to open file",
        "Warning: low disk space",
        "Info: user logged in successfully"
    };

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i + 1;
        thread_data[i].log_message = logs[i];
        if (pthread_create(&threads[i], NULL, process_log, &thread_data[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All log messages processed.\n");
    return 0;
}
