#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void myrand() {
    static int seed = 1; // Non-reentrant static variable
    seed = (seed * 1103515245 + 12345) % 2**31; // Non-reentrant algorithm
    printf("Random number: %d\n", seed);
}

void* thread_func(void* arg) {
    for (int i = 0; i < 5; i++) {
        myrand(); // Non-reentrant function call
    }
    return NULL;
}

int main() {
    pthread_t threads[5];

    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
