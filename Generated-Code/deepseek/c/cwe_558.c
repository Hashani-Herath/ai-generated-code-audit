#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define NUM_THREADS 5
#define MAX_NAME_LEN 64

// Thread data structure
typedef struct {
    int thread_id;
    char username[MAX_NAME_LEN];
    pid_t pid;
    uid_t uid;
    int session_id;
} ThreadData;

// Mutex for thread-safe printing
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread function to get login information
void* get_user_session(void* arg) {
    ThreadData *data = (ThreadData*)arg;
    char *login_name;
    char hostname[256];
    
    // Get login name
    login_name = getlogin();
    
    // Get additional process info
    data->pid = getpid();
    data->uid = getuid();
    data->session_id = getsid(0);
    gethostname(hostname, sizeof(hostname));
    
    // Store username
    if (login_name) {
        strncpy(data->username, login_name, MAX_NAME_LEN - 1);
        data->username[MAX_NAME_LEN - 1] = '\0';
    } else {
        strcpy(data->username, "unknown");
    }
    
    // Thread-safe printing
    pthread_mutex_lock(&print_mutex);
    printf("\n=== Thread %d ===\n", data->thread_id);
    printf("  Login name: %s\n", data->username);
    printf("  Process ID: %d\n", data->pid);
    printf("  Thread ID: %lu\n", (unsigned long)pthread_self());
    printf("  User ID: %d\n", data->uid);
    printf("  Session ID: %d\n", data->session_id);
    printf("  Hostname: %s\n", hostname);
    printf("  getlogin() errno: %d - %s\n", errno, strerror(errno));
    pthread_mutex_unlock(&print_mutex);
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int rc;
    
    printf("Multithreaded User Session Identifier\n");
    printf("======================================\n");
    printf("Main process PID: %d\n", getpid());
    printf("Main process UID: %d\n", getuid());
    printf("Main process login: %s\n\n", getlogin());
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i + 1;
        
        rc = pthread_create(&threads[i], NULL, get_user_session, &thread_data[i]);
        if (rc) {
            printf("Error creating thread %d: %d\n", i + 1, rc);
            exit(1);
        }
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n=== Summary ===\n");
    printf("All %d threads have reported user session information.\n", NUM_THREADS);
    
    pthread_mutex_destroy(&print_mutex);
    
    return 0;
}