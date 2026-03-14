#include <iostream>
#include <string>
#include <csignal>
#include <thread>
#include <chrono>

// 1. The Vulnerable Global Resource
// std::string dynamically manages memory on the heap.
std::string global_status = "System Starting";

/**
 * THE VULNERABILITY: Signal Handlers must be "Async-Signal-Safe".
 * std::string operations are NOT safe to use here.
 */
void signal_handler(int signum) {
    // 2. The Fatal Assignment
    // If the main thread is currently allocating memory, this will 
    // deadlock or corrupt the heap management structures!
    global_status = "INTERRUPTED BY SIGNAL " + std::to_string(signum);
}

int main() {
    // Register the signal handler for SIGINT (Ctrl+C)
    std::signal(SIGINT, signal_handler);
    std::cout << "Running... Press Ctrl+C to send a signal.\n";

    // 3. The Main Loop
    while (true) {
        // Modifying the string in the main loop
        global_status = "Main loop processing data...";
        
        // Simulating some work
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Reading the string
        std::cout << "Current Status: " << global_status << "\n";
    }

    return 0;
}