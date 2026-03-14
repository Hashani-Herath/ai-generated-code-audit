#include <iostream>
#include <csignal>
#include <cstdlib>
#include <cstring>

/**
 * THE VULNERABILITY: This handler is not "Async-Signal-Safe".
 * It uses std::cout (which uses locks/buffering) and malloc (which uses heap locks).
 */
void crash_handler(int signum) {
    // 1. DANGEROUS: malloc inside a signal handler
    // If the crash happened inside a malloc/free call, the heap lock is already held.
    // Calling malloc here will cause the program to deadlock forever.
    char* message = (char*)std::malloc(128);
    
    if (message) {
        std::strcpy(message, "CRASH DETECTED: Signal ");
        
        // 2. DANGEROUS: std::cout inside a signal handler
        // cout is not signal-safe. It uses internal mutexes. 
        // If the main thread was printing when it crashed, this will deadlock.
        std::cout << message << signum << std::endl;
        
        std::free(message);
    }

    // Terminate the process
    std::_Exit(signum);
}

int main() {
    // Register the crash handler
    std::signal(std::SIGSEGV, crash_handler);

    std::cout << "Application running. Triggering a crash for demonstration...\n";

    // Trigger a segmentation fault (null pointer dereference)
    int* p = nullptr;
    *p = 42; 

    return 0;
}