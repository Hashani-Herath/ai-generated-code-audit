#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>
#include <condition_variable>

/**
 * Global shared resource
 */
int shared_resource = 0;

/**
 * Synchronization primitives
 */
std::mutex mtx;                      // For method 1
std::atomic<int> atomic_resource(0);  // For method 2
std::recursive_mutex rec_mtx;         // For method 3 (if needed)
std::shared_mutex shared_mtx;          // For reader-writer scenarios

/**
 * Statistics for each thread
 */
struct ThreadStats {
    int thread_id;
    int increments;
    std::chrono::microseconds total_time;
    int conflicts;
    
    ThreadStats() : thread_id(0), increments(0), total_time(0), conflicts(0) {}
};

std::vector<ThreadStats> thread_stats;

/**
 * Method 1: Using mutex (safe, correct)
 */
void increment_with_mutex(int thread_id, int iterations) {
    ThreadStats stats;
    stats.thread_id = thread_id;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        // Lock mutex before accessing shared resource
        mtx.lock();
        shared_resource++;  // Critical section
        mtx.unlock();
        stats.increments++;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    stats.total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    thread_stats[thread_id] = stats;
}

/**
 * Method 1b: Using lock_guard (RAII, safer)
 */
void increment_with_lock_guard(int thread_id, int iterations) {
    ThreadStats stats;
    stats.thread_id = thread_id;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        std::lock_guard<std::mutex> lock(mtx);  // Automatically unlocks
        shared_resource++;  // Critical section
        stats.increments++;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    stats.total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    thread_stats[thread_id] = stats;
}

/**
 * Method 2: Using atomic operations (lock-free)
 */
void increment_with_atomic(int thread_id, int iterations) {
    ThreadStats stats;
    stats.thread_id = thread_id;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        atomic_resource.fetch_add(1, std::memory_order_relaxed);
        stats.increments++;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    stats.total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    thread_stats[thread_id] = stats;
}

/**
 * Method 3: Using unique_lock (more flexible)
 */
void increment_with_unique_lock(int thread_id, int iterations) {
    ThreadStats stats;
    stats.thread_id = thread_id;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        shared_resource++;
        stats.increments++;
        lock.unlock();  // Can manually unlock if needed
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    stats.total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    thread_stats[thread_id] = stats;
}

/**
 * Method 4: Using try_lock (handles contention)
 */
void increment_with_trylock(int thread_id, int iterations) {
    ThreadStats stats;
    stats.thread_id = thread_id;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        while (!mtx.try_lock()) {
            stats.conflicts++;
            std::this_thread::yield();  // Give other threads a chance
        }
        shared_resource++;
        mtx.unlock();
        stats.increments++;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    stats.total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    thread_stats[thread_id] = stats;
}

/**
 * WRONG: No synchronization (data race)
 */
void increment_without_sync(int thread_id, int iterations) {
    ThreadStats stats;
    stats.thread_id = thread_id;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        shared_resource++;  // DATA RACE - UNSAFE!
        stats.increments++;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    stats.total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    thread_stats[thread_id] = stats;
}

/**
 * Worker function that can use different increment methods
 */
void worker(int thread_id, int iterations, int method) {
    switch (method) {
        case 0:
            increment_with_mutex(thread_id, iterations);
            break;
        case 1:
            increment_with_lock_guard(thread_id, iterations);
            break;
        case 2:
            increment_with_atomic(thread_id, iterations);
            break;
        case 3:
            increment_with_unique_lock(thread_id, iterations);
            break;
        case 4:
            increment_with_trylock(thread_id, iterations);
            break;
        case 99:  // Special case to show data race
            increment_without_sync(thread_id, iterations);
            break;
        default:
            increment_with_lock_guard(thread_id, iterations);
    }
}

/**
 * Print thread statistics
 */
void print_stats(const std::string& method_name, int expected_total) {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "Method: " << method_name << "\n";
    std::cout << std::string(50, '=') << "\n";
    
    int total_increments = 0;
    auto total_time = std::chrono::microseconds(0);
    int total_conflicts = 0;
    
    for (const auto& stats : thread_stats) {
        std::cout << "Thread " << stats.thread_id << ": "
                  << stats.increments << " increments in "
                  << stats.total_time.count() << " µs";
        if (stats.conflicts > 0) {
            std::cout << " (" << stats.conflicts << " conflicts)";
        }
        std::cout << "\n";
        
        total_increments += stats.increments;
        total_time += stats.total_time;
        total_conflicts += stats.conflicts;
    }
    
    std::cout << "----------------------------------------\n";
    std::cout << "Total increments: " << total_increments << "\n";
    std::cout << "Expected total: " << expected_total << "\n";
    std::cout << "Difference: " << (expected_total - total_increments) << "\n";
    std::cout << "Total conflicts: " << total_conflicts << "\n";
    
    if (total_increments == expected_total) {
        std::cout << "✓ CORRECT - No race conditions\n";
    } else {
        std::cout << "✗ WRONG - Race condition detected!\n";
    }
}

/**
 * Run test with specified method
 */
void run_test(const std::string& method_name, int method, int iterations_per_thread = 100000) {
    // Reset shared resources
    shared_resource = 0;
    atomic_resource = 0;
    
    // Prepare thread stats
    thread_stats.clear();
    thread_stats.resize(4);
    
    // Create threads
    std::vector<std::thread> threads;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(worker, i, iterations_per_thread, method);
    }
    
    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Get final value based on method
    int final_value = (method == 2) ? atomic_resource.load() : shared_resource;
    
    print_stats(method_name, iterations_per_thread * 4);
    std::cout << "Total execution time: " << total_duration.count() << " ms\n";
    std::cout << "Final shared_resource value: " << final_value << "\n";
}

/**
 * Demonstrate data race (unsafe)
 */
void demonstrate_data_race() {
    std::cout << "\n\n=== DEMONSTRATING DATA RACE (UNSAFE) ===\n";
    std::cout << "WARNING: This shows what happens WITHOUT synchronization!\n\n";
    
    // Reset
    shared_resource = 0;
    
    // Create threads without synchronization
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 100000; ++j) {
                shared_resource++;  // DATA RACE!
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Expected final value: 400000\n";
    std::cout << "Actual final value:   " << shared_resource << "\n";
    std::cout << "Difference: " << (400000 - shared_resource) << "\n";
    std::cout << "✗ DATA RACE DETECTED! Value is incorrect.\n";
}

/**
 * Demonstrate different memory orders with atomics
 */
void demonstrate_memory_orders() {
    std::cout << "\n\n=== Atomic Memory Orders ===\n";
    
    std::atomic<int> counter{0};
    
    auto relaxed_increment = [&counter](int iterations) {
        for (int i = 0; i < iterations; ++i) {
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    };
    
    auto seq_cst_increment = [&counter](int iterations) {
        for (int i = 0; i < iterations; ++i) {
            counter.fetch_add(1, std::memory_order_seq_cst);
        }
    };
    
    // Test relaxed
    counter = 0;
    std::vector<std::thread> threads;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(relaxed_increment, 100000);
    }
    for (auto& t : threads) t.join();
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << "Relaxed ordering: " << counter.load() 
              << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms\n";
    
    // Test sequential consistency
    counter = 0;
    threads.clear();
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(seq_cst_increment, 100000);
    }
    for (auto& t : threads) t.join();
    end = std::chrono::high_resolution_clock::now();
    
    std::cout << "Sequential consistent: " << counter.load() 
              << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms\n";
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Multithreaded Increment Demonstration\n";
    std::cout << "4 threads incrementing shared resource\n";
    std::cout << "========================================\n";
    
    try {
        // First, demonstrate the problem (data race)
        demonstrate_data_race();
        
        // Now show various correct solutions
        std::cout << "\n\n=== CORRECT SOLUTIONS ===\n";
        
        // Method 0: Mutex with manual lock/unlock
        run_test("Mutex (manual lock/unlock)", 0);
        
        // Method 1: Lock guard (RAII)
        run_test("Lock Guard (RAII)", 1);
        
        // Method 2: Atomic operations
        run_test("Atomic Operations", 2);
        
        // Method 3: Unique lock
        run_test("Unique Lock", 3);
        
        // Method 4: Try lock with yield
        run_test("Try Lock with Yield", 4);
        
        // Demonstrate memory orders
        demonstrate_memory_orders();
        
        // Summary
        std::cout << "\n=== Key Takeaways ===\n";
        std::cout << "1. Always synchronize access to shared data\n";
        std::cout << "2. std::mutex provides safe mutual exclusion\n";
        std::cout << "3. std::atomic provides lock-free synchronization\n";
        std::cout << "4. RAII locks (lock_guard) prevent forgotten unlocks\n";
        std::cout << "5. Data races cause undefined behavior\n";
        std::cout << "6. Always verify thread safety in multithreaded code\n";
        
        std::cout << "\n=== Performance Notes ===\n";
        std::cout << "- Mutex:   Safe, moderate overhead\n";
        std::cout << "- Atomic:  Usually faster for simple operations\n";
        std::cout << "- TryLock: May be better under high contention\n";
        std::cout << "- No sync: Fast but WRONG (data race)\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}