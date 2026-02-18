#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <string>
#include <chrono>
#include <random>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

/**
 * Custom exception for server errors
 */
class ServerError : public std::runtime_error {
public:
    explicit ServerError(const std::string& message) 
        : std::runtime_error("Server Error: " + message) {}
};

/**
 * Thread-safe logger class
 */
class Logger {
private:
    std::mutex log_mutex;
    std::ofstream log_file;
    std::chrono::steady_clock::time_point start_time;
    
public:
    Logger(const std::string& filename = "server.log") {
        log_file.open(filename, std::ios::app);
        if (!log_file.is_open()) {
            throw ServerError("Failed to open log file");
        }
        start_time = std::chrono::steady_clock::now();
    }
    
    ~Logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
    
    void log(const std::string& message, int thread_id = -1) {
        std::lock_guard<std::mutex> lock(log_mutex);
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start_time).count();
        
        std::stringstream ss;
        ss << "[" << std::setw(4) << elapsed << "ms] ";
        if (thread_id >= 0) {
            ss << "[Thread " << std::setw(2) << thread_id << "] ";
        }
        ss << message;
        
        std::string full_message = ss.str();
        
        // Write to console
        std::cout << full_message << std::endl;
        
        // Write to file
        log_file << full_message << std::endl;
        log_file.flush();
    }
};

/**
 * Thread-safe user information cache
 */
class UserInfoCache {
private:
    struct UserInfo {
        std::string username;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::mutex cache_mutex;
    std::unordered_map<uid_t, UserInfo> cache;
    std::chrono::seconds cache_duration;
    
public:
    UserInfoCache(std::chrono::seconds duration = std::chrono::seconds(60))
        : cache_duration(duration) {}
    
    std::string getUsername(uid_t uid) {
        std::lock_guard<std::mutex> lock(cache_mutex);
        
        auto now = std::chrono::steady_clock::now();
        auto it = cache.find(uid);
        
        if (it != cache.end()) {
            auto age = now - it->second.timestamp;
            if (age < cache_duration) {
                return it->second.username;
            }
        }
        
        // Not in cache or expired, get from system
        std::string username = getUsernameFromSystem(uid);
        
        // Update cache
        cache[uid] = {username, now};
        
        return username;
    }
    
private:
    std::string getUsernameFromSystem(uid_t uid) {
        struct passwd* pw = getpwuid(uid);
        if (pw) {
            return std::string(pw->pw_name);
        }
        return "unknown";
    }
};

/**
 * Thread-safe request queue
 */
class RequestQueue {
private:
    std::queue<int> requests;
    std::mutex queue_mutex;
    std::condition_variable cv;
    bool shutdown_flag;
    
public:
    RequestQueue() : shutdown_flag(false) {}
    
    void push(int request_id) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        requests.push(request_id);
        cv.notify_one();
    }
    
    bool pop(int& request_id) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        cv.wait(lock, [this]() { 
            return !requests.empty() || shutdown_flag; 
        });
        
        if (shutdown_flag && requests.empty()) {
            return false;
        }
        
        request_id = requests.front();
        requests.pop();
        return true;
    }
    
    void shutdown() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        shutdown_flag = true;
        cv.notify_all();
    }
    
    size_t size() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return requests.size();
    }
};

/**
 * Server class with thread pool
 */
class MultiThreadedServer {
private:
    int port;
    int num_threads;
    std::vector<std::thread> thread_pool;
    RequestQueue request_queue;
    Logger logger;
    UserInfoCache user_cache;
    std::atomic<bool> running;
    std::atomic<int> request_counter;
    uid_t server_uid;
    
public:
    MultiThreadedServer(int port_number, int thread_count) 
        : port(port_number)
        , num_threads(thread_count)
        , running(false)
        , request_counter(0)
        , server_uid(getuid()) {
        
        logger.log("Server initializing on port " + std::to_string(port) + 
                   " with " + std::to_string(thread_count) + " threads");
        
        // Get server owner username
        std::string owner = getProcessOwner();
        logger.log("Server process owner: " + owner + " (UID: " + 
                   std::to_string(server_uid) + ")");
    }
    
    ~MultiThreadedServer() {
        shutdown();
    }
    
    std::string getProcessOwner() {
        return user_cache.getUsername(server_uid);
    }
    
    std::string getProcessOwnerDirect() {
        // Direct call to getlogin() - may be cached
        const char* login = getlogin();
        if (login) {
            return std::string(login);
        }
        
        // Fallback to getpwuid
        struct passwd* pw = getpwuid(getuid());
        if (pw) {
            return std::string(pw->pw_name);
        }
        
        return "unknown";
    }
    
    void start() {
        running = true;
        logger.log("Server starting...");
        
        // Create thread pool
        for (int i = 0; i < num_threads; ++i) {
            thread_pool.emplace_back(&MultiThreadedServer::workerThread, this, i);
        }
        
        // Simulate incoming requests
        std::thread acceptor(&MultiThreadedServer::requestAcceptor, this);
        acceptor.detach();
    }
    
    void shutdown() {
        if (!running) return;
        
        logger.log("Server shutting down...");
        running = false;
        request_queue.shutdown();
        
        // Wait for all threads to finish
        for (auto& thread : thread_pool) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        logger.log("Server shutdown complete");
    }
    
private:
    void workerThread(int thread_id) {
        logger.log("Worker thread " + std::to_string(thread_id) + " started", thread_id);
        
        // Get process owner username using different methods
        std::string owner1 = getProcessOwner();
        std::string owner2 = getProcessOwnerDirect();
        
        logger.log("Process owner (from cache): " + owner1, thread_id);
        logger.log("Process owner (from getlogin): " + owner2, thread_id);
        
        while (running) {
            int request_id;
            if (request_queue.pop(request_id)) {
                processRequest(thread_id, request_id);
            }
        }
        
        logger.log("Worker thread " + std::to_string(thread_id) + " stopping", thread_id);
    }
    
    void requestAcceptor() {
        logger.log("Request acceptor started");
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> delay(100, 500);
        std::uniform_int_distribution<> request_type(0, 3);
        
        while (running) {
            int request_id = ++request_counter;
            int type = request_type(gen);
            
            logger.log("Accepted request #" + std::to_string(request_id) + 
                       " (type " + std::to_string(type) + ")");
            
            request_queue.push(request_id);
            
            // Random delay between requests
            std::this_thread::sleep_for(std::chrono::milliseconds(delay(gen)));
        }
        
        logger.log("Request acceptor stopping");
    }
    
    void processRequest(int thread_id, int request_id) {
        logger.log("Processing request #" + std::to_string(request_id), thread_id);
        
        // Get process owner information multiple times to demonstrate
        std::string owner = getProcessOwner();
        std::string login = getProcessOwnerDirect();
        
        logger.log("Request #" + std::to_string(request_id) + 
                   " - Process owner: " + owner + " (UID: " + 
                   std::to_string(server_uid) + ")", thread_id);
        
        // Simulate different request types
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> work_time(100, 500);
        
        // Simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(work_time(gen)));
        
        // Log completion with owner info
        logger.log("Request #" + std::to_string(request_id) + 
                   " completed (owner: " + login + ")", thread_id);
    }
};

/**
 * Test server with different configurations
 */
void testServer(int num_threads, int duration_seconds) {
    std::cout << "\n=== Testing Server with " << num_threads 
              << " threads for " << duration_seconds << " seconds ===\n";
    
    MultiThreadedServer server(8080, num_threads);
    
    server.start();
    
    // Let it run for specified duration
    std::this_thread::sleep_for(std::chrono::seconds(duration_seconds));
    
    server.shutdown();
}

/**
 * Demonstrate getlogin() behavior in multithreaded context
 */
void demonstrateGetloginBehavior() {
    std::cout << "\n=== getlogin() Behavior in Multithreaded Context ===\n";
    
    Logger logger;
    
    // Get process owner information from main thread
    uid_t uid = getuid();
    struct passwd* pw = getpwuid(uid);
    const char* login = getlogin();
    
    logger.log("Process UID: " + std::to_string(uid));
    if (pw) {
        logger.log("Username from getpwuid: " + std::string(pw->pw_name));
        logger.log("User home: " + std::string(pw->pw_dir));
        logger.log("User shell: " + std::string(pw->pw_shell));
    }
    
    if (login) {
        logger.log("Username from getlogin: " + std::string(login));
    } else {
        logger.log("getlogin() returned nullptr - check permissions");
    }
    
    // Spawn multiple threads calling getlogin()
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&logger, i, &success_count]() {
            for (int j = 0; j < 10; ++j) {
                const char* login = getlogin();
                if (login) {
                    success_count++;
                    logger.log("Thread " + std::to_string(i) + 
                               " getlogin(): " + std::string(login), i);
                } else {
                    logger.log("Thread " + std::to_string(i) + 
                               " getlogin() failed", i);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    logger.log("getlogin() successful in " + std::to_string(success_count.load()) + 
               "/100 calls");
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Multithreaded Server with getlogin()\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Note: getlogin() returns the username associated with the\n";
    cout << "calling process's login session. It may return NULL if:\n";
    cout << "  - Process is not associated with a terminal\n";
    cout << "  - Running as daemon\n";
    cout << "  - Permissions issue\n\n";
    
    try {
        // Demonstrate getlogin() behavior
        demonstrateGetloginBehavior();
        
        // Test server with different thread counts
        testServer(2, 5);   // 2 threads for 5 seconds
        testServer(4, 5);   // 4 threads for 5 seconds
        testServer(8, 5);   // 8 threads for 5 seconds
        
        // Additional test with custom configuration
        std::cout << "\n=== Custom Server Test ===\n";
        MultiThreadedServer custom_server(9090, 6);
        custom_server.start();
        
        // Let it run for a bit
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        // Summary
        std::cout << "\n=== getlogin() Thread Safety Notes ===\n";
        std::cout << "✓ getlogin() is typically thread-safe on modern systems\n";
        std::cout << "✓ Results may be cached by the C library\n";
        std::cout << "✓ Returns same value for all threads in process\n";
        std::cout << "✓ Consider caching to reduce system calls\n";
        std::cout << "✓ Fallback to getpwuid() if getlogin() fails\n";
        
        // Clean shutdown
        custom_server.shutdown();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}