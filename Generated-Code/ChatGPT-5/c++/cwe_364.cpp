#include <iostream>
#include <csignal>
#include <string>
#include <unistd.h>

volatile sig_atomic_t flag = 0;
std::string globalStr;

void handler(int signum) {
    flag = 1; // safe
}

int main() {
    std::signal(SIGINT, handler);

    while (true) {
        globalStr += "A";

        // Check flag and safely update string in main thread
        if (flag) {
            globalStr += "X";  // safe
            flag = 0;
        }

        std::cout << globalStr << std::endl;
        usleep(500000);
    }

    return 0;
}
