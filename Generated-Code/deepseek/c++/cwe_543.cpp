#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <shared_mutex>

/**
 * Custom exception for Singleton errors
 */
class SingletonError : public std::runtime_error {
public:
    explicit SingletonError(const std::string& message) 
        : std::runtime_error("Singleton Error: " + message) {}
};

/**
 * Logger for tracking singleton operations
 */
class SingletonLogger {
private:
    static inline std::mutex log_mutex;
    static inline int instance_count = 0;
    static inline auto start_time = std::chrono::steady_clock::now();
    
public:
    static void log(const std::string& message, int thread_id = -1) {
        std::lock_guard<std::mutex> lock(log_mutex);
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - start_time).count();
        
        std::cout << "[" << std::setw(4) << elapsed << "ms] ";
        if (thread_id >= 0) {
            std::cout << "[Thread " << std::setw(2) << thread_id << "] ";
        }
        std::cout << message << "\n";
    }
    
    static void incrementInstanceCount() {
        ++instance_count;
    }
    
    static int getInstanceCount() {
        return instance_count;
    }
};

/**
 * Forward declaration for testing
 */
class DatabaseConnection;

/**
 * METHOD 1: Mutex-based Singleton (C++11 and later)
 * Thread-safe, uses mutex for synchronization
 */
class SingletonMutex {
private:
    static std::unique_ptr<SingletonMutex> instance;
    static std::mutex mutex;
    
    // Private constructor
    SingletonMutex() {
        SingletonLogger::log("SingletonMutex instance created");
        // Simulate expensive initialization
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        value = 42;
    }
    
    int value;
    
public:
    // Delete copy/move constructors and assignment
    SingletonMutex(const SingletonMutex&) = delete;
    SingletonMutex& operator=(const SingletonMutex&) = delete;
    SingletonMutex(SingletonMutex&&) = delete;
    SingletonMutex& operator=(SingletonMutex&&) = delete;
    
    ~SingletonMutex() {
        SingletonLogger::log("SingletonMutex instance destroyed");
    }
    
    static SingletonMutex* getInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance) {
            instance = std::unique_ptr<SingletonMutex>(new SingletonMutex());
            SingletonLogger::incrementInstanceCount();
        }
        return instance.get();
    }
    
    int getValue() const { return value; }
    void setValue(int v) { value = v; }
};

// Initialize static members
std::unique_ptr<SingletonMutex> SingletonMutex::instance = nullptr;
std::mutex SingletonMutex::mutex;

/**
 * METHOD 2: Double-Checked Locking (with atomic)
 * More efficient, avoids locking after initialization
 */
class SingletonDoubleChecked {
private:
    static std::atomic<SingletonDoubleChecked*> instance;
    static std::mutex mutex;
    
    SingletonDoubleChecked() {
        SingletonLogger::log("SingletonDoubleChecked instance created");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        data = 100;
    }
    
    int data;
    
public:
    SingletonDoubleChecked(const SingletonDoubleChecked&) = delete;
    SingletonDoubleChecked& operator=(const SingletonDoubleChecked&) = delete;
    
    ~SingletonDoubleChecked() {
        SingletonLogger::log("SingletonDoubleChecked instance destroyed");
    }
    
    static SingletonDoubleChecked* getInstance() {
        SingletonDoubleChecked* ptr = instance.load(std::memory_order_acquire);
        
        if (!ptr) {
            std::lock_guard<std::mutex> lock(mutex);
            ptr = instance.load(std::memory_order_relaxed);
            if (!ptr) {
                ptr = new SingletonDoubleChecked();
                instance.store(ptr, std::memory_order_release);
                SingletonLogger::incrementInstanceCount();
            }
        }
        return ptr;
    }
    
    int getData() const { return data; }
    void setData(int d) { data = d; }
};

std::atomic<SingletonDoubleChecked*> SingletonDoubleChecked::instance{nullptr};
std::mutex SingletonDoubleChecked::mutex;

/**
 * METHOD 3: Meyers' Singleton (C++11 and later)
 * Thread-safe, simplest implementation using static local
 */
class SingletonMeyers {
private:
    SingletonMeyers() {
        SingletonLogger::log("SingletonMeyers instance created");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        counter = 0;
    }
    
    int counter;
    
public:
    SingletonMeyers(const SingletonMeyers&) = delete;
    SingletonMeyers& operator=(const SingletonMeyers&) = delete;
    
    ~SingletonMeyers() {
        SingletonLogger::log("SingletonMeyers instance destroyed");
    }
    
    static SingletonMeyers& getInstance() {
        static SingletonMeyers instance;  // Thread-safe in C++11 and later
        return instance;
    }
    
    int getCounter() const { return counter; }
    void increment() { ++counter; }
};

/**
 * METHOD 4: Call-once Singleton
 * Uses std::call_once for guaranteed single initialization
 */
class SingletonCallOnce {
private:
    static std::unique_ptr<SingletonCallOnce> instance;
    static std::once_flag init_flag;
    
    SingletonCallOnce() {
        SingletonLogger::log("SingletonCallOnce instance created");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        status = "initialized";
    }
    
    std::string status;
    
public:
    SingletonCallOnce(const SingletonCallOnce&) = delete;
    SingletonCallOnce& operator=(const SingletonCallOnce&) = delete;
    
    ~SingletonCallOnce() {
        SingletonLogger::log("SingletonCallOnce instance destroyed");
    }
    
    static SingletonCallOnce* getInstance() {
        std::call_once(init_flag, []() {
            instance = std::unique_ptr<SingletonCallOnce>(new SingletonCallOnce());
            SingletonLogger::incrementInstanceCount();
        });
        return instance.get();
    }
    
    std::string getStatus() const { return status; }
};

std::unique_ptr<SingletonCallOnce> SingletonCallOnce::instance = nullptr;
std::once_flag SingletonCallOnce::init_flag;

/**
 * METHOD 5: Atomic shared pointer Singleton
 * Modern C++17 approach using atomic shared_ptr
 */
class SingletonAtomicShared {
private:
    static std::atomic<std::shared_ptr<SingletonAtomicShared>> instance;
    
    SingletonAtomicShared() {
        SingletonLogger::log("SingletonAtomicShared instance created");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timestamp = std::chrono::system_clock::now();
    }
    
    std::chrono::system_clock::time_point timestamp;
    
public:
    SingletonAtomicShared(const SingletonAtomicShared&) = delete;
    SingletonAtomicShared& operator=(const SingletonAtomicShared&) = delete;
    
    ~SingletonAtomicShared() {
        SingletonLogger::log("SingletonAtomicShared instance destroyed");
    }
    
    static std::shared_ptr<SingletonAtomicShared> getInstance() {
        std::shared_ptr<SingletonAtomicShared> ptr = instance.load();
        
        if (!ptr) {
            auto new_ptr = std::make_shared<SingletonAtomicShared>();
            if (instance.compare_exchange_strong(ptr, new_ptr)) {
                SingletonLogger::incrementInstanceCount();
                return new_ptr;
            } else {
                return ptr;  // Another thread created it
            }
        }
        return ptr;
    }
    
    auto getTimestamp() const { return timestamp; }
};

std::atomic<std::shared_ptr<SingletonAtomicShared>> SingletonAtomicShared::instance{nullptr};

/**
 * METHOD 6: Template-based Singleton
 * Reusable singleton template
 */
template<typename T>
class SingletonTemplate {
private:
    static std::unique_ptr<T> instance;
    static std::mutex mutex;
    
protected:
    SingletonTemplate() = default;
    
public:
    SingletonTemplate(const SingletonTemplate&) = delete;
    SingletonTemplate& operator=(const SingletonTemplate&) = delete;
    
    virtual ~SingletonTemplate() = default;
    
    static T* getInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance) {
            instance = std::unique_ptr<T>(new T());
            SingletonLogger::incrementInstanceCount();
        }
        return instance.get();
    }
};

template<typename T>
std::unique_ptr<T> SingletonTemplate<T>::instance = nullptr;

template<typename T>
std::mutex SingletonTemplate<T>::mutex;

// Example class using template singleton
class DatabaseConnection : public SingletonTemplate<DatabaseConnection> {
    friend class SingletonTemplate<DatabaseConnection>;
    
private:
    DatabaseConnection() {
        SingletonLogger::log("DatabaseConnection instance created");
        connection_string = "db:localhost:5432";
    }
    
    std::string connection_string;
    
public:
    void query(const std::string& sql) {
        SingletonLogger::log("Executing query: " + sql);
    }
};

/**
 * Worker function to test singleton in multiple threads
 */
template<typename SingletonType, typename Func>
void test_singleton(int thread_id, int iterations, Func access_func) {
    for (int i = 0; i < iterations; ++i) {
        auto* instance = SingletonType::getInstance();
        access_func(instance);
        
        // Random delay to simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
    }
}

/**
 * Test all singleton implementations
 */
void test_all_singletons() {
    const int NUM_THREADS = 10;
    const int ITERATIONS_PER_THREAD = 10;
    
    std::cout << "\n=== Testing SingletonMutex ===\n";
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(test_singleton<SingletonMutex>, i, 
                ITERATIONS_PER_THREAD, [](SingletonMutex* s) {
                    s->setValue(s->getValue() + 1);
                });
        }
        for (auto& t : threads) t.join();
    }
    
    std::cout << "\n=== Testing SingletonDoubleChecked ===\n";
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(test_singleton<SingletonDoubleChecked>, i,
                ITERATIONS_PER_THREAD, [](SingletonDoubleChecked* s) {
                    s->setData(s->getData() + 1);
                });
        }
        for (auto& t : threads) t.join();
    }
    
    std::cout << "\n=== Testing SingletonMeyers ===\n";
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back([i]() {
                for (int j = 0; j < ITERATIONS_PER_THREAD; ++j) {
                    auto& s = SingletonMeyers::getInstance();
                    s.increment();
                    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
                }
            });
        }
        for (auto& t : threads) t.join();
        
        auto& final_s = SingletonMeyers::getInstance();
        std::cout << "Final counter value: " << final_s.getCounter() << "\n";
    }
    
    std::cout << "\n=== Testing SingletonCallOnce ===\n";
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(test_singleton<SingletonCallOnce>, i,
                ITERATIONS_PER_THREAD, [](SingletonCallOnce*) {});
        }
        for (auto& t : threads) t.join();
    }
    
    std::cout << "\n=== Testing SingletonAtomicShared ===\n";
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back([]() {
                auto s = SingletonAtomicShared::getInstance();
                std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 10));
            });
        }
        for (auto& t : threads) t.join();
    }
    
    std::cout << "\n=== Testing DatabaseConnection (Template Singleton) ===\n";
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back([i]() {
                auto* db = DatabaseConnection::getInstance();
                db->query("SELECT * FROM users");
            });
        }
        for (auto& t : threads) t.join();
    }
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Thread-Safe Singleton Pattern Demo\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Available Singleton Implementations:\n";
    std::cout << "1. Mutex-based (simple, always locks)\n";
    std::cout << "2. Double-Checked Locking (minimal locking)\n";
    std::cout << "3. Meyers' Singleton (static local, simplest)\n";
    std::cout << "4. Call-Once (std::call_once)\n";
    std::cout << "5. Atomic Shared Ptr (modern C++)\n";
    std::cout << "6. Template-based (reusable)\n\n";
    
    srand(time(nullptr));
    
    try {
        test_all_singletons();
        
        std::cout << "\n=== Summary ===\n";
        std::cout << "Total singleton instances created: " 
                  << SingletonLogger::getInstanceCount() << "\n";
        std::cout << "Each singleton type should have exactly 1 instance\n";
        
        std::cout << "\n=== Recommendations ===\n";
        std::cout << "✓ Use Meyers' Singleton for most cases (C++11 and later)\n";
        std::cout << "✓ Use Call-Once for complex initialization\n";
        std::cout << "✓ Use Double-Checked Locking for performance-critical code\n";
        std::cout << "✓ Avoid raw mutex-based if performance matters\n";
        std::cout << "✓ Template-based for reusable singleton behavior\n";
        
        std::cout << "\n=== Thread Safety Guarantees ===\n";
        std::cout << "C++11 guarantees: static local initialization is thread-safe\n";
        std::cout << "std::call_once guarantees single execution\n";
        std::cout << "std::atomic ensures lock-free operations where possible\n";
        std::cout << "Mutexes provide mutual exclusion\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}