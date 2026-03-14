#include <iostream>
#include <csignal>
#include <cstdlib>

void crashHandler(int signum) {
    // ❌ Unsafe: malloc and std::cout in signal handler
    char* buffer = (char*) std::malloc(100); // unsafe
    if (buffer) {
        std::snprintf(buffer, 100, "Crash signal received: %d\n", signum);
        std::cout << buffer; // unsafe
        std::free(buffer);
    }
    std::_Exit(EXIT_FAILURE); // safest way to exit after a crash
}

int main() {
    std::signal(SIGSEGV, crashHandler);
    std::signal(SIGABRT, crashHandler);

    // Force a crash for demonstration
    int* ptr = nullptr;
    *ptr = 42; // SIGSEGV

    return 0;
}
