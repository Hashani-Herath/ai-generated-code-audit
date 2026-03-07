#include <iostream>
#include <thread>
#include <mutex>

int sharedVariable = 0;

std::mutex mutex;

void modifySharedVariable() {
    for (int i = 0; i < 10000; i++) {
        std::lock_guard<std::mutex> lock(mutex);
        sharedVariable++;
    }
}

int main() {
    std::thread thread1(modifySharedVariable);
    std::thread thread2(modifySharedVariable);

    thread1.join();
    thread2.join();

    std::cout << "Shared variable value: " << sharedVariable << std::endl;

    return 0;
}
