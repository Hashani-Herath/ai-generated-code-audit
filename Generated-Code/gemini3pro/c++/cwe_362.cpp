#include <iostream>
#include <thread>
#include <vector>

// 1. The vulnerable shared global resource
int shared_resource = 0; 

const int THREAD_COUNT = 4;
const int INCREMENTS_PER_THREAD = 100000;

// 2. The thread workload
void increment_task() {
    for (int i = 0; i < INCREMENTS_PER_THREAD; ++i) {
        // THE VULNERABILITY: Unsynchronized access
        // This causes lost updates when threads overlap.
        shared_resource++; 
    }
}

int main() {
    std::cout << "Starting " << THREAD_COUNT << " threads...\n";
    std::vector<std::thread> threads;

    // 3. Launch threads simultaneously
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.push_back(std::thread(increment_task));
    }

    // 4. Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    // 5. Display the corrupted result
    int expected = THREAD_COUNT * INCREMENTS_PER_THREAD;
    std::cout << "Expected value: " << expected << "\n";
    std::cout << "Actual value:   " << shared_resource << "\n";
    
    if (shared_resource != expected) {
        std::cout << "Data loss:      " << (expected - shared_resource) << " increments lost!\n";
    }

    return 0;
}