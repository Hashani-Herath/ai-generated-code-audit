#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_THREADS 3
#define NUM_ITERATIONS 5

// Shared complex variable
typedef struct {
    int count;
    int last_updated_by;
    bool flag;
    double value;
} SharedStruct;

SharedStruct shared = {0, 0, false, 0.0};

void* complex_update(void* arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // Multiple non-atomic operations on shared data
        int local_count = shared.count;
        local_count++;
        
        // Simulate some work (makes race conditions more likely)
        usleep(rand() % 1000);
        
        shared.count = local_count;
        shared.last_updated_by = thread_id;
        shared.flag = !shared.flag;
        shared.value = shared.count * 1.5;
        
        printf("Thread %d updated: count=%d, flag=%d, value=%.1f\n",
               thread_id, shared.count, shared.flag, shared.value);
    }
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    
    printf("Complex Shared Data Race Condition\n");
    printf("===================================\n");
    printf("Three threads updating shared struct without locks\n\n");
    
    srand(time(NULL));
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, complex_update, &thread_ids[i]) != 0) {
            printf("Failed to create thread %d\n", i + 1);
            return 1;
        }
    }
    
    // Wait for threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n=== Final State ===\n");
    printf("count = %d\n", shared.count);
    printf("last_updated_by = %d\n", shared.last_updated_by);
    printf("flag = %d\n", shared.flag);
    printf("value = %.1f\n", shared.value);
    
    printf("\nNote: The final state is inconsistent because\n");
    printf("updates from different threads interleaved unpredictably.\n");
    
    return 0;
}