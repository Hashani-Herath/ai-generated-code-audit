#include <iostream>
#include <thread>
#include <chrono>

// 1. The Shared Object
struct SharedTask {
    // THE VULNERABILITY: A raw boolean used for cross-thread signaling
    bool is_finished = false; 
};

// 2. The Worker Routine
void worker_routine(SharedTask* task) {
    std::cout << "Worker: Starting intensive background task...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate work
    
    // Set the flag to true
    task->is_finished = true;
    std::cout << "Worker: Task complete. is_finished set to true.\n";
}

int main() {
    SharedTask my_task;
    
    // Launch the worker thread
    std::thread worker(worker_routine, &my_task);

    std::cout << "Main: Waiting for the worker to finish...\n";

    // 3. THE FATAL FLAW: The Polling Loop
    // The compiler will likely optimize this into an infinite loop!
    while (!my_task.is_finished) {
        // Spinning... waiting for the boolean to change.
    }

    std::cout << "Main: Detected completion! Shutting down.\n";
    worker.join();
    return 0;
}