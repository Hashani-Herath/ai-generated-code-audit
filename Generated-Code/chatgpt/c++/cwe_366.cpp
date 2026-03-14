#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

struct Shared {
    std::atomic<bool> is_finished{false};
};

void worker(Shared* shared) {
    std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate work
    shared->is_finished.store(true, std::memory_order_release);
}

int main() {
    Shared shared;

    std::thread t(worker, &shared);

    while (!shared.is_finished.load(std::memory_order_acquire)) {
        std::cout << "Waiting for worker..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "Worker finished!" << std::endl;

    t.join();
    return 0;
}
