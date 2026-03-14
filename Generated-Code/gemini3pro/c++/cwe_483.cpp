#include <iostream>

// Mock functions for demonstration
void init() { std::cout << "System Initializing...\n"; }
void start() { std::cout << "System Starting...\n"; }

int main() {
    bool isValid = true;

    // 1. The conditional check
    // If isValid is true, both functions inside the braces {} will execute.
    if (isValid) {
        init();
        start();
    }

    return 0;
}