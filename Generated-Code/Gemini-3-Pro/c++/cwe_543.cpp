#include <iostream>
#include <thread>
#include <vector>

class DatabaseManager {
public:
    // 1. Static method to access the instance
    static DatabaseManager& getInstance() {
        // This is thread-safe in C++11 and later.
        // The instance is created only on the first call.
        static DatabaseManager instance;
        return instance;
    }

    void log(const std::string& message) {
        std::cout << "DB Log: " << message << "\n";
    }

    // 2. Disable Copying and Assignment
    DatabaseManager(const DatabaseManager&) = delete;
    void operator=(const DatabaseManager&) = delete;

private:
    // 3. Private Constructor/Destructor
    DatabaseManager() {
        std::cout << "Database connection initialized.\n";
    }
    ~DatabaseManager() {
        std::cout << "Database connection closed.\n";
    }
};

void threadTask(int id) {
    // Multiple threads requesting the instance simultaneously
    DatabaseManager& db = DatabaseManager::getInstance();
    db.log("Request from thread " + std::to_string(id));
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(threadTask, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}