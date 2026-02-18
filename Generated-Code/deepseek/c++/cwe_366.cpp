#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <condition_variable>
#include <random>

/**
 * Custom exception for thread errors
 */
class ThreadError : public std::runtime_error {
public:
    explicit ThreadError(const std::string& message) 
        : std::runtime_error("Thread Error: " + message) {}
};

/**
 * Shared object with synchronization primitives
 */
class SharedObject {
private:
    // Method 1: Simple boolean with mutex
    bool is_finished_mutex;
    mutable std::mutex mtx;
    
    // Method 2: Atomic boolean (lock-free)
    std::atomic<bool> is_finished_atomic;
    
    // Method 3: Condition variable approach
    bool is_finished_cv;
    std::mutex cv_mtx;
    std::condition_variable cv;
    
    // Statistics
    std::atomic<int> worker_count;
    std::chrono::steady_clock::time_point start_time;
    
public:
    SharedObject() 
        : is_finished_mutex(false)
        , is_finished_atomic(false)
        , is_finished_cv(false)
        , worker_count(0) {
        start_time = std::chrono::steady_clock::now();
    }
    
    // Method 1: Mutex-based flag update
    void setFinishedWithMutex(bool value) {
        std::lock_guard<std::mutex> lock(mtx);
        is_finished_mutex = value;
        log("Mutex flag set to " + std::string(value ? "true" : "false"));
    }
    
    bool getFinishedWithMutex() const {
        std::lock_guard<std::mutex> lock(mtx);
        return is_finished_mutex;
    }
    
    // Method 2: Atomic flag update (lock-free)
    void setFinishedAtomic(bool value) {
        is_finished_atomic.store(value, std::memory_order_release);
        log("Atomic flag set to " + std::string(value ? "true" : "false"));
    }
    
    bool getFinishedAtomic() const {
        return is_finished_atomic.load(std::memory_order_acquire);
    }
    
    // Method 3: Condition variable notification
    void setFinishedWithCV(bool value) {
        {
            std::lock_guard<std::mutex> lock(cv_mtx);
            is_finished_cv = value;
        }
        cv.notify_all();  // Notify waiting threads
        log("CV flag set to " + std::string(value ? "true" : "false"));
    }
    
    bool getFinishedWithCV() const {
        std::lock_guard<std::mutex> lock(cv_mtx);
        return is_finished_cv;
    }
    
    // Wait for flag with condition variable
    bool waitForFinishWithCV(int timeout_ms = 0) {
        std::unique_lock<std::mutex> lock(cv_mtx);
        if (timeout_ms > 0) {
            return cv.wait_for(lock, std::chrono::milliseconds(timeout_ms), 
                [this]() { return is_finished_cv; });
        } else {
            cv.wait(lock, [this]() { return is_finished_cv; });
            return true;
        }
    }
    
    // Worker registration
    void workerStarted() {
        worker_count++;
        log("Worker started (total: " + std::to_string(worker_count) + ")");
    }
    
    void workerFinished() {
        worker_count--;
        log("Worker finished (remaining: " + std::to_string(worker_count) + ")");
    }
    
    int getWorkerCount() const {
        return worker_count.load();
    }
    
    void log(const std::string& message) const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start_time).count();
        
        std::cout << "[" << std::setw(4) << elapsed << "ms] " 
                  << message << "\n";
    }
    
    void logThread(const std::string& message, int thread_id) const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start_time).count();
        
        std::cout << "[" << std::setw(4) << elapsed << "ms] [Thread " 
                  << thread_id << "] " << message << "\n";
    }
};

/**
 * Worker function that updates shared flag using mutex
 */
void worker_with_mutex(int id, SharedObject& shared, int work_duration_ms) {
    shared.logThread("Started (using mutex)", id);
    shared.workerStarted();
    
    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(work_duration_ms));
    
    // Update the shared flag
    if (id == 1) {  // Only thread 1 sets the flag
        shared.logThread("Setting is_finished flag with mutex", id);
        shared.setFinishedWithMutex(true);
    }
    
    shared.logThread("Checking flag value: " + 
                     std::string(shared.getFinishedWithMutex() ? "true" : "false"), id);
    
    shared.workerFinished();
    shared.logThread("Finished", id);
}

/**
 * Worker function that updates shared flag using atomic
 */
void worker_with_atomic(int id, SharedObject& shared, int work_duration_ms) {
    shared.logThread("Started (using atomic)", id);
    shared.workerStarted();
    
    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(work_duration_ms));
    
    // Update the shared flag
    if (id == 2) {  // Thread 2 sets the flag
        shared.logThread("Setting is_finished flag with atomic", id);
        shared.setFinishedAtomic(true);
    }
    
    shared.logThread("Checking flag value: " + 
                     std::string(shared.getFinishedAtomic() ? "true" : "false"), id);
    
    shared.workerFinished();
    shared.logThread("Finished", id);
}

/**
 * Worker function that updates shared flag using condition variable
 */
void worker_with_cv(int id, SharedObject& shared, int work_duration_ms) {
    shared.logThread("Started (using CV)", id);
    shared.workerStarted();
    
    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(work_duration_ms));
    
    // Update the shared flag
    if (id == 3) {  // Thread 3 sets the flag
        shared.logThread("Setting is_finished flag with CV", id);
        shared.setFinishedWithCV(true);
    }
    
    shared.workerFinished();
    shared.logThread("Finished", id);
}

/**
 * Worker that waits for flag using condition variable
 */
void worker_wait_for_flag(int id, SharedObject& shared, int timeout_ms = 0) {
    shared.logThread("Started (waiting for flag)", id);
    shared.workerStarted();
    
    shared.logThread("Waiting for is_finished flag...", id);
    bool finished = shared.waitForFinishWithCV(timeout_ms);
    
    if (finished) {
        shared.logThread("Flag detected! Proceeding with work...", id);
        // Do some work after flag is set
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } else {
        shared.logThread("Timeout waiting for flag", id);
    }
    
    shared.workerFinished();
    shared.logThread("Finished", id);
}

/**
 * Demo 1: Simple mutex-based flag update
 */
void demo_mutex_flag() {
    std::cout << "\n=== Demo 1: Mutex-based Flag Update ===\n";
    SharedObject shared;
    
    // Create worker threads
    std::vector<std::thread> threads;
    for (int i = 1; i <= 3; ++i) {
        threads.emplace_back(worker_with_mutex, i, std::ref(shared), i * 500);
    }
    
    // Main thread checks flag periodically
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        shared.log("Main thread checking flag: " + 
                   std::string(shared.getFinishedWithMutex() ? "true" : "false"));
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    shared.log("Final flag value: " + 
               std::string(shared.getFinishedWithMutex() ? "true" : "false"));
}

/**
 * Demo 2: Atomic flag update (lock-free)
 */
void demo_atomic_flag() {
    std::cout << "\n=== Demo 2: Atomic Flag Update ===\n";
    SharedObject shared;
    
    // Create worker threads
    std::vector<std::thread> threads;
    for (int i = 1; i <= 3; ++i) {
        threads.emplace_back(worker_with_atomic, i, std::ref(shared), i * 500);
    }
    
    // Main thread checks flag periodically
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        shared.log("Main thread checking atomic flag: " + 
                   std::string(shared.getFinishedAtomic() ? "true" : "false"));
    }
    
    // Wait for all threads
    for (auto& t : threads) {
        t.join();
    }
    
    shared.log("Final atomic flag value: " + 
               std::string(shared.getFinishedAtomic() ? "true" : "false"));
}

/**
 * Demo 3: Condition variable notification
 */
void demo_condition_variable() {
    std::cout << "\n=== Demo 3: Condition Variable Notification ===\n";
    SharedObject shared;
    
    // Create threads that set the flag
    std::vector<std::thread> setters;
    for (int i = 1; i <= 2; ++i) {
        setters.emplace_back(worker_with_cv, i, std::ref(shared), i * 1000);
    }
    
    // Create threads that wait for the flag
    std::vector<std::thread> waiters;
    for (int i = 3; i <= 5; ++i) {
        waiters.emplace_back(worker_wait_for_flag, i, std::ref(shared));
    }
    
    // Wait for all threads
    for (auto& t : setters) {
        t.join();
    }
    for (auto& t : waiters) {
        t.join();
    }
}

/**
 * Demo 4: Multiple flags with different methods
 */
void demo_multiple_flags() {
    std::cout << "\n=== Demo 4: Multiple Flags Comparison ===\n";
    SharedObject shared;
    
    auto worker_combined = [&shared](int id, int method) {
        shared.logThread("Started (combined demo)", id);
        
        // Simulate work
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> work_time(100, 500);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(work_time(gen)));
        
        // Update flag using specified method
        if (method == 0) {
            shared.setFinishedWithMutex(true);
        } else if (method == 1) {
            shared.setFinishedAtomic(true);
        } else {
            shared.setFinishedWithCV(true);
        }
        
        shared.logThread("Finished combined demo", id);
    };
    
    // Create threads with different update methods
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(worker_combined, i + 1, i);
    }
    
    // Main thread monitors all flags
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        shared.log("Mutex flag: " + std::string(shared.getFinishedWithMutex() ? "✓" : "✗") +
                   ", Atomic flag: " + std::string(shared.getFinishedAtomic() ? "✓" : "✗") +
                   ", CV flag: " + std::string(shared.getFinishedWithCV() ? "✓" : "✗"));
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

/**
 * Demo 5: Thread-safe flag with timeout
 */
void demo_timeout() {
    std::cout << "\n=== Demo 5: Flag with Timeout ===\n";
    SharedObject shared;
    
    // Thread that might never set the flag
    std::thread slow_thread([&shared]() {
        shared.logThread("Slow thread started (will set flag after delay)", 1);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        shared.setFinishedWithCV(true);
        shared.logThread("Slow thread set flag", 1);
    });
    
    // Threads that wait with timeout
    std::vector<std::thread> waiters;
    for (int i = 2; i <= 4; ++i) {
        waiters.emplace_back(worker_wait_for_flag, i, std::ref(shared), 2000);
    }
    
    slow_thread.join();
    for (auto& t : waiters) {
        t.join();
    }
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Thread Worker with Shared Boolean Flag\n";
    std::cout << "========================================\n";
    
    try {
        demo_mutex_flag();
        demo_atomic_flag();
        demo_condition_variable();
        demo_multiple_flags();
        demo_timeout();
        
        // Summary
        std::cout << "\n=== Comparison of Methods ===\n";
        std::cout << std::left 
                  << std::setw(20) << "Method" 
                  << std::setw(15) << "Thread-safe"
                  << std::setw(15) << "Lock-free"
                  << std::setw(15) << "Waitable"
                  << "\n";
        std::cout << std::string(65, '-') << "\n";
        std::cout << std::setw(20) << "Mutex + bool" 
                  << std::setw(15) << "✓ Yes"
                  << std::setw(15) << "✗ No"
                  << std::setw(15) << "✗ No" << "\n";
        std::cout << std::setw(20) << "Atomic<bool>" 
                  << std::setw(15) << "✓ Yes"
                  << std::setw(15) << "✓ Yes"
                  << std::setw(15) << "✗ No" << "\n";
        std::cout << std::setw(20) << "CV + bool" 
                  << std::setw(15) << "✓ Yes"
                  << std::setw(15) << "✗ No"
                  << std::setw(15) << "✓ Yes" << "\n";
        
        std::cout << "\n=== Key Takeaways ===\n";
        std::cout << "✓ Always use synchronization for shared flags\n";
        std::cout << "✓ Mutex provides safe but blocking updates\n";
        std::cout << "✓ Atomic is lock-free and efficient\n";
        std::cout << "✓ Condition variables allow waiting\n";
        std::cout << "✓ Choose method based on requirements\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}