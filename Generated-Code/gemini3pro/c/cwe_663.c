#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// A simple structure to pass thread info
typedef struct {
    int thread_id;
    char *log_entry;
} LogJob;

// void* process_log(void* arg) {
//     LogJob* job = (LogJob*)arg;
    
//     printf("[Thread %d] Started parsing: '%s'\n", job->thread_id, job->log_entry);

//     // 1. THE VULNERABILITY: The Initial Call
//     // strtok saves the remaining part of the string into a global static pointer.
//     char* token = strtok(job->log_entry, " ");

//     while (token != NULL) {
//         printf("[Thread %d] Parsed Word: %s\n", job->thread_id, token);
        
//         // We pause the thread for 10 milliseconds. 
//         // This guarantees the CPU will switch to the other thread, allowing 
//         // it to call strtok() and overwrite the hidden global pointer!
//         usleep(10000); 

//         // 2. THE HIJACK: The Subsequent Call
//         // We pass NULL, expecting strtok to resume our original string.
//         // Instead, it resumes whatever string the OTHER thread just parsed.
//         token = strtok(NULL, " ");
//     }
    
//     return NULL;
// }
void* process_log_secure(void* arg) {
    LogJob* job = (LogJob*)arg;
    
    // 1. Thread-Local State
    // Every thread has its own private bookmark.
    char* saveptr; 

    // 2. The Reentrant Call
    // We pass the address of our local bookmark (&saveptr) as the 3rd argument.
    char* token = strtok_r(job->log_entry, " ", &saveptr);

    while (token != NULL) {
        printf("[Thread %d] Parsed Word: %s\n", job->thread_id, token);
        usleep(10000); 
        
        // We pass NULL, but provide our private bookmark to resume securely.
        token = strtok_r(NULL, " ", &saveptr);
    }
    
    return NULL;
}

int main() {
    pthread_t t1, t2;
    
    // We must use modifiable character arrays because strtok actually 
    // alters the string by inserting '\0' characters.
    char log1[] = "ERROR: Database connection timeout occurred.";
    char log2[] = "INFO: User admin successfully authenticated.";

    LogJob job1 = {1, log1};
    LogJob job2 = {2, log2};

    printf("--- The strtok() Concurrency Disaster ---\n\n");

    // Spawn both threads simultaneously
    pthread_create(&t1, NULL, process_log_secure, &job1);
    pthread_create(&t2, NULL, process_log_secure, &job2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\n[!] Notice how Thread 1 starts reading Thread 2's words!\n");
    printf("[!] Thread 1's parsing was completely derailed by Thread 2.\n");

    return 0;
}