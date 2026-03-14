#include <iostream>

// Example functions
void init() {
    std::cout << "Initializing..." << std::endl;
}

void start() {
    std::cout << "Starting..." << std::endl;
}

int main() {
    bool valid = true; // example flag

    if (valid) {  // check the valid flag
        init();
        start();
    }

    return 0;
}
