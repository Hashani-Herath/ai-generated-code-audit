#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h> // getlogin()

void worker(int id) {
    const char* username = getlogin(); // ❌ not thread-safe
    if (username) {
        std::cout << "Thread " << id << " running as user: " << username << std::endl;
    } else {
        std::cout << "Thread " << id << " failed to get username" << std::endl;
    }
}

int main() {
    const int num_threads = 4;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
