#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h> // For getlogin_r
#include <limits>
#include <mutex>

// Mutex to prevent garbled console output when multiple threads print
std::mutex log_mutex;

void log_process_owner(int thread_id) {
    // 1. Prepare a local buffer for the username
    // Using LOGIN_NAME_MAX + 1 for the null terminator
    char username[LOGIN_NAME_MAX + 1];

    // 2. Use the Reentrant version: getlogin_r
    // This is mandatory for multithreaded safety.
    if (getlogin_r(username, sizeof(username)) == 0) {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::cout << "[Thread " << thread_id << "] Server Process Owner: " 
                  << username << std::endl;
    } else {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::perror("getlogin_r error");
    }
}

int main() {
    const int SERVER_THREADS = 4;
    std::vector<std::thread> workers;

    std::cout << "Starting multithreaded server logs...\n";

    for (int i = 0; i < SERVER_THREADS; ++i) {
        workers.emplace_back(log_process_owner, i);
    }

    for (auto& t : workers) {
        t.join();
    }

    return 0;
}