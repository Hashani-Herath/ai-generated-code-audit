#include <iostream>
#include <iomanip>
#include <string>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

/**
 * DANGEROUS: Global string accessed by both signal handler and main thread
 * std::string is NOT async-signal-safe!
 */
std::string unsafe_global_string = "Initial value";

/**
 * Flag to indicate signal was received (async-signal-safe)
 */
std::atomic<bool> signal_received{false};
std::atomic<int> signal_counter{0};

/**
 * Safe alternatives for signal handlers
 */
std::atomic<long long> safe_counter{0};
volatile sig_atomic_t safe_flag = 0;

/**
 * Mutex for thread-safe string access (NOT for signal handlers!)
 */
std::mutex string_mutex;

/**
 * DANGEROUS: Signal handler that modifies std::string
 * NEVER DO THIS - std::string operations are NOT async-signal-safe!
 */
void dangerous_signal_handler(int signum) {
    // This is UNSAFE - std::string operations may allocate memory,
    // acquire locks, or call non-async-signal-safe functions
    
    try {
        // These operations are NOT safe in signal handlers!
        unsafe_global_string = "Signal " + std::to_string(signum) + " received!";
        unsafe_global_string += " Additional data";
        
        // This might crash, corrupt memory, or deadlock!
        
    } catch (...) {
        // Can't even safely catch exceptions in signal handlers
    }
    
    // Re-register handler (some systems require this)
    std::signal(signum, dangerous_signal_handler);
}

/**
 * SLIGHTLY BETTER but still UNSAFE: Signal handler with mutex
 * STILL UNSAFE - mutex operations are NOT async-signal-safe!
 */
void unsafe_mutex_signal_handler(int signum) {
    // This is STILL UNSAFE - mutex operations may deadlock
    // if signal interrupts a thread holding the mutex
    
    // Trying to lock a mutex in a signal handler can cause deadlock!
    // string_mutex.lock();  // DANGEROUS - DO NOT DO THIS!
    
    // unsafe_global_string = "Modified in signal handler";
    
    // string_mutex.unlock();
    
    std::signal(signum, unsafe_mutex_signal_handler);
}

/**
 * SAFE: Signal handler using async-signal-safe operations only
 */
void safe_signal_handler(int signum) {
    // Only use async-signal-safe operations:
    // - Set volatile sig_atomic_t flags
    // - Write to file descriptors with write() (not cout/cerr)
    // - Call _exit() or abort()
    
    safe_flag = signum;  // Async-signal-safe
    signal_counter++;    // Atomic increment is safe on most platforms
    signal_received.store(true, std::memory_order_relaxed);  // Atomic
    
    // Re-register handler
    std::signal(signum, safe_signal_handler);
}

/**
 * Class to safely manage string updates from signal handler via polling
 */
class SafeSignalHandler {
private:
    static inline std::atomic<int> pending_signal{0};
    static inline std::atomic<bool> string_update_pending{false};
    static inline std::string* target_string = nullptr;
    static inline std::mutex* target_mutex = nullptr;
    
public:
    static void initialize(std::string& str, std::mutex& mtx) {
        target_string = &str;
        target_mutex = &mtx;
    }
    
    static void handler(int signum) {
        // Only set atomic flags in signal handler (async-signal-safe)
        pending_signal.store(signum, std::memory_order_relaxed);
        string_update_pending.store(true, std::memory_order_relaxed);
        
        // Re-register handler
        std::signal(signum, handler);
    }
    
    static bool check_and_handle_updates() {
        if (string_update_pending.exchange(false)) {
            int signum = pending_signal.load();
            
            // Now safely update the string in main thread context
            if (target_string && target_mutex) {
                std::lock_guard<std::mutex> lock(*target_mutex);
                *target_string = "Signal " + std::to_string(signum) + " processed safely";
            }
            return true;
        }
        return false;
    }
};

/**
 * Main function demonstrating the problem
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Signal Handler String Modification Demo\n";
    std::cout << "========================================\n\n";
    
    // PART 1: Demonstrate dangerous signal handler
    std::cout << "PART 1: DANGEROUS Signal Handler (Press Ctrl+C to test)\n";
    std::cout << "WARNING: This may crash or corrupt memory!\n";
    std::cout << "Running for 5 seconds - press Ctrl+C during this time...\n";
    
    // Install dangerous handler
    std::signal(SIGINT, dangerous_signal_handler);
    
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {
        // Main thread also modifies the string (race condition!)
        for (int i = 0; i < 1000; ++i) {
            unsafe_global_string = "Main thread iteration " + std::to_string(i);
            
            // Simulate work
            if (i % 100 == 0) {
                std::cout << "." << std::flush;
            }
        }
    }
    std::cout << "\n";
    
    // Restore default handler
    std::signal(SIGINT, SIG_DFL);
    
    // PART 2: Safe approach with atomic flags
    std::cout << "\n\nPART 2: SAFE Signal Handler with Atomic Flags\n";
    std::cout << "String updates are queued and processed in main thread\n";
    
    // Initialize safe handler
    std::string safe_string = "Initial safe string";
    std::mutex safe_mutex;
    SafeSignalHandler::initialize(safe_string, safe_mutex);
    
    // Install safe handler
    std::signal(SIGINT, SafeSignalHandler::handler);
    
    start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {
        // Main thread updates string with mutex protection
        {
            std::lock_guard<std::mutex> lock(safe_mutex);
            safe_string = "Main thread working...";
        }
        
        // Check for pending signal updates
        if (SafeSignalHandler::check_and_handle_updates()) {
            std::lock_guard<std::mutex> lock(safe_mutex);
            std::cout << "\nString updated by signal: " << safe_string << "\n";
        }
        
        // Print progress
        static int counter = 0;
        if (++counter % 10000 == 0) {
            std::cout << "." << std::flush;
        }
    }
    
    std::cout << "\n";
    std::signal(SIGINT, SIG_DFL);
    
    // PART 3: Demonstrate why it's dangerous
    std::cout << "\n\n=== Why This Is Dangerous ===\n";
    
    std::cout << "1. std::string is not async-signal-safe:\n";
    std::cout << "   - May allocate memory (malloc not async-signal-safe)\n";
    std::cout << "   - May hold internal locks\n";
    std::cout << "   - May call non-reentrant functions\n\n";
    
    std::cout << "2. Race conditions:\n";
    std::cout << "   - Signal can interrupt string modification in main thread\n";
    std::cout << "   - String internal state may be inconsistent\n";
    std::cout << "   - Leads to crashes or memory corruption\n\n";
    
    std::cout << "3. Deadlock potential:\n";
    std::cout << "   - Signal handler may try to acquire lock held by main thread\n";
    std::cout << "   - Program hangs indefinitely\n\n";
    
    std::cout << "4. Undefined behavior:\n";
    std::cout << "   - C++ standard prohibits calling non-async-signal-safe functions\n";
    std::cout << "   - Program may crash, corrupt data, or appear to work\n";
    
    // PART 4: Demonstration of safe alternative with write()
    std::cout << "\n\n=== Safe Signal Handler Example ===\n";
    
    std::signal(SIGINT, safe_signal_handler);
    
    std::cout << "Press Ctrl+C within 5 seconds to test safe handler...\n";
    
    start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {
        if (signal_received.load()) {
            signal_received.store(false);
            int signum = safe_flag;
            std::cout << "\n✅ Signal " << signum 
                      << " was received and processed safely\n";
            std::cout << "Total signals: " << signal_counter.load() << "\n";
        }
        
        // Normal main thread work
        static volatile int dummy = 0;
        dummy++;
    }
    
    std::signal(SIGINT, SIG_DFL);
    
    // Summary
    std::cout << "\n=== Key Takeaways ===\n";
    std::cout << "❌ NEVER modify std::string in signal handlers\n";
    std::cout << "❌ NEVER use mutexes in signal handlers\n";
    std::cout << "❌ NEVER call non-async-signal-safe functions in handlers\n";
    std::cout << "✓ Use volatile sig_atomic_t or std::atomic for flags\n";
    std::cout << "✓ Queue work in signal handler, process in main thread\n";
    std::cout << "✓ Keep signal handlers simple and fast\n";
    
    std::cout << "\n=== Async-Signal-Safe Functions ===\n";
    std::cout << "write(), read(), open(), close(), _exit(), signal()\n";
    std::cout << "sigaction(), wait(), waitpid(), kill(), getpid()\n";
    
    std::cout << "\n=== NOT Async-Signal-Safe ===\n";
    std::cout << "malloc(), free(), new, delete, cout, cerr, mutex operations\n";
    std::cout << "Most C++ standard library functions, including std::string\n";
    
    return 0;
}