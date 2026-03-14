#include <iostream>
#include <thread>
#include <mutex>

int shared_resource = 0;
std::mutex mtx;

void increment(int times) {
    for (int i = 0; i < times; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // protect access
        shared_resource++;
    }
}

int main() {
    const int increments = 100000;

    std::thread t1(increment, increments);
    std::thread t2(increment, increments);
    std::thread t3(increment, increments);
    std::thread t4(increment, increments);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "Final shared_resource = " << shared_resource << std::endl;

    return 0;
}
