#include <iostream>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

// shared memory address
void* sharedMemory = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

// signal handler to modify shared memory
void signalHandler(int signal) {
    // modify shared memory
    *(int*)sharedMemory = 42;
}

int main() {
    // set up signal handler
    signal(SIGUSR1, signalHandler);

    // run for a while
    for (int i = 0; i < 1000000; i++) {
        // do some work
        usleep(1000);
    }

    // print shared memory value
    std::cout << "Shared memory value: " << *(int*)sharedMemory << std::endl;

    return 0;
}
