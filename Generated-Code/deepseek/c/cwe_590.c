#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

// Thread-safe memory manager
typedef struct {
    pthread_mutex_t mutex;
    void **pointers;
    size_t *sizes;
    int count;
    int capacity;
} ThreadSafeMemoryManager;

ThreadSafeMemoryManager* create_memory_manager(int capacity) {
    ThreadSafeMemoryManager *mgr = (ThreadSafeMemoryManager*)malloc(sizeof(ThreadSafeMemoryManager));
    if (!mgr) return NULL;
    
    pthread_mutex_init(&mgr->mutex, NULL);
    mgr->pointers = (void**)malloc(capacity * sizeof(void*));
    mgr->sizes = (size_t*)malloc(capacity * sizeof(size_t));
    
    if (!mgr->pointers || !mgr->sizes) {
        free(mgr->pointers);
        free(mgr->sizes);
        free(mgr);
        return NULL;
    }
    
    mgr->count = 0;
    mgr->capacity = capacity;
    
    return mgr;
}

void destroy_memory_manager(ThreadSafeMemoryManager *mgr) {
    if (!mgr) return;
    
    pthread_mutex_destroy(&mgr->mutex);
    free(mgr->pointers);
    free(mgr->sizes);
    free(mgr);
}

bool manager_add_pointer(ThreadSafeMemoryManager *mgr, void *ptr, size_t size) {
    if (!mgr || !ptr) return false;
    
    pthread_mutex_lock(&mgr->mutex);
    
    if (mgr->count < mgr->capacity) {
        mgr->pointers[mgr->count] = ptr;
        mgr->sizes[mgr->count] = size;
        mgr->count++;
        pthread_mutex_unlock(&mgr->mutex);
        return true;
    }
    
    pthread_mutex_unlock(&mgr->mutex);
    return false;
}

bool manager_remove_pointer(ThreadSafeMemoryManager *mgr, void *ptr) {
    if (!mgr || !ptr) return false;
    
    pthread_mutex_lock(&mgr->mutex);
    
    for (int i = 0; i < mgr->count; i++) {
        if (mgr->pointers[i] == ptr) {
            // Remove by swapping with last element
            mgr->pointers[i] = mgr->pointers[mgr->count - 1];
            mgr->sizes[i] = mgr->sizes[mgr->count - 1];
            mgr->count--;
            pthread_mutex_unlock(&mgr->mutex);
            return true;
        }
    }
    
    pthread_mutex_unlock(&mgr->mutex);
    return false;
}

size_t manager_get_size(ThreadSafeMemoryManager *mgr, void *ptr) {
    size_t size = 0;
    
    if (!mgr || !ptr) return 0;
    
    pthread_mutex_lock(&mgr->mutex);
    
    for (int i = 0; i < mgr->count; i++) {
        if (mgr->pointers[i] == ptr) {
            size = mgr->sizes[i];
            break;
        }
    }
    
    pthread_mutex_unlock(&mgr->mutex);
    return size;
}

// Thread-safe free function
typedef struct {
    int success;
    size_t freed_size;
    char message[256];
} ThreadSafeFreeResult;

ThreadSafeFreeResult thread_safe_free(ThreadSafeMemoryManager *mgr, void **ptr_ptr) {
    ThreadSafeFreeResult result = {0};
    
    if (!ptr_ptr) {
        snprintf(result.message, sizeof(result.message), "Error: NULL pointer to pointer");
        return result;
    }
    
    void *ptr = *ptr_ptr;
    
    if (!ptr) {
        snprintf(result.message, sizeof(result.message), "Warning: Pointer already NULL");
        result.success = 0;
        return result;
    }
    
    // Get size from manager
    size_t size = manager_get_size(mgr, ptr);
    
    // Secure clear
    if (size > 0) {
        memset(ptr, 0, size);
        result.freed_size = size;
    }
    
    // Free memory
    free(ptr);
    
    // Remove from manager
    manager_remove_pointer(mgr, ptr);
    
    // Set pointer to NULL
    *ptr_ptr = NULL;
    
    snprintf(result.message, sizeof(result.message), 
             "Successfully freed %zu bytes", size);
    result.success = 1;
    
    return result;
}

// Thread function
void* worker_thread(void* arg) {
    ThreadSafeMemoryManager *mgr = (ThreadSafeMemoryManager*)arg;
    char *buffer = malloc(50);
    
    if (buffer) {
        strcpy(buffer, "Thread-local data");
        manager_add_pointer(mgr, buffer, 50);
        
        printf("Thread %lu: allocated buffer at %p\n", 
               (unsigned long)pthread_self(), buffer);
        
        // Simulate work
        sleep(1);
        
        ThreadSafeFreeResult r = thread_safe_free(mgr, (void**)&buffer);
        printf("Thread %lu: %s\n", (unsigned long)pthread_self(), r.message);
    }
    
    return NULL;
}

int main() {
    pthread_t threads[3];
    ThreadSafeMemoryManager *mgr = create_memory_manager(100);
    
    printf("Thread-Safe Memory Free Utility\n");
    printf("===============================\n\n");
    
    // Create threads that allocate and free memory
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, worker_thread, mgr);
    }
    
    // Wait for threads
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    destroy_memory_manager(mgr);
    
    return 0;
}