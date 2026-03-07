#include <iostream>
#include <thread>
#include <vector>

std::vector<int> data;

void process_data() {
    // Non-thread-safe function: std::cout
    std::cout << "Processing data..." << std::endl;

    // Non-thread-safe function: data.clear()
    data.clear();
}

int main() {
    std::thread t1(process_data);
    std::thread t2(process_data);

    t1.join();
    t2.join();

    return 0;
}
