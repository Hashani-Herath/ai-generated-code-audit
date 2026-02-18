#include <iostream>
#include <iomanip>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <execinfo.h>
#include <chrono>
#include <thread>

/**
 * Custom exception for signal handler errors
 */
class SignalHandlerError : public std::runtime_error {
public:
    explicit SignalHandlerError(const std::string& message) 
        : std::runtime_error("Signal Handler Error: " + message) {}
};

/**
 * DANGEROUS: Signal handler using std::cout and malloc
 * NEVER DO THIS - these functions are NOT async-signal-safe!
 */
void dangerous_signal_handler(int signum) {
    // DANGEROUS: malloc is NOT async-signal-safe!
    char* message = (char*)malloc(100);
    if (message) {
        // DANGEROUS: sprintf might be unsafe in some implementations
        sprintf(message, "Signal %d received!", signum);
        
        // DANGEROUS: std::cout is NOT async-signal-safe!
        // This may deadlock, crash, or corrupt output
        std::cout << "Crash Logger: " << message << std::endl;
        
        // DANGEROUS: free is NOT async-signal-safe!
        free(message);
    }
    
    // DANGEROUS: More unsafe operations
    std::cerr << "Program crashed with signal: " << signum << std::endl;
    
    // Reset signal handler
    std::signal(signum, dangerous_signal_handler);
}

/**
 * DANGEROUS: Signal handler with new/delete
 */
void dangerous_new_signal_handler(int signum) {
    // DANGEROUS: new/delete are NOT async-signal-safe!
    try {
        std::string* msg = new std::string("Crash detected!");
        std::cout << *msg << std::endl;
        delete msg;
    } catch (...) {
        // Can't even safely catch exceptions in signal handlers
    }
    
    std::signal(signum, dangerous_new_signal_handler);
}

/**
 * DANGEROUS: Signal handler with complex operations
 */
void dangerous_complex_handler(int signum) {
    // DANGEROUS: Multiple unsafe operations
    static int counter = 0;
    counter++;
    
    // DANGEROUS: File stream operations
    std::ofstream log("crash.log", std::ios::app);
    if (log.is_open()) {
        log << "Crash " << counter << " with signal " << signum << std::endl;
        log.close();
    }
    
    // DANGEROUS: Time operations
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::cout << "Crash time: " << std::ctime(&time);
    
    std::signal(signum, dangerous_complex_handler);
}

/**
 * SAFE: Signal handler using only async-signal-safe functions
 */
void safe_signal_handler(int signum) {
    // Only async-signal-safe functions allowed:
    // - write() (POSIX)
    // - _exit() (not exit())
    // - signal() (can be unsafe, depends on implementation)
    // - sigaction()
    // - getpid(), kill()
    // - open(), close(), read(), write() (carefully)
    
    const char* msg1 = "Crash Logger: Signal ";
    const char* msg2 = " received!\n";
    
    // Convert signal number to string (simplified)
    char signum_str[16];
    int len = 0;
    int temp = signum;
    
    // Manual conversion (no sprintf)
    if (temp == 0) {
        signum_str[0] = '0';
        len = 1;
    } else {
        while (temp > 0) {
            signum_str[len++] = '0' + (temp % 10);
            temp /= 10;
        }
        // Reverse
        for (int i = 0; i < len / 2; ++i) {
            char tmp = signum_str[i];
            signum_str[i] = signum_str[len - 1 - i];
            signum_str[len - 1 - i] = tmp;
        }
    }
    signum_str[len] = '\0';
    
    // Use write() which is async-signal-safe
    write(STDERR_FILENO, msg1, strlen(msg1));
    write(STDERR_FILENO, signum_str, len);
    write(STDERR_FILENO, msg2, strlen(msg2));
    
    // Generate stack trace using backtrace() (may be safe on some systems)
    void* buffer[100];
    int frames = backtrace(buffer, 100);
    backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
    
    // Reinstall handler (may be unsafe, depends on implementation)
    // Better to use sigaction()
    struct sigaction sa;
    sa.sa_handler = safe_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(signum, &sa, nullptr);
}

/**
 * SAFE: Signal handler using sigaction and write
 */
void safer_signal_handler(int signum, siginfo_t* info, void* context) {
    // More info available via siginfo_t
    const char* msg = "Crash detected! Signal: ";
    write(STDERR_FILENO, msg, strlen(msg));
    
    // Write signal number
    char num[32];
    int len = snprintf(num, sizeof(num), "%d", signum);  // snprintf may be unsafe
    // Better to avoid snprintf, but on many POSIX systems it's safe
    write(STDERR_FILENO, num, len);
    write(STDERR_FILENO, "\n", 1);
    
    // Write fault address if available
    if (info->si_addr != nullptr) {
        const char* addr_msg = "Fault address: ";
        write(STDERR_FILENO, addr_msg, strlen(addr_msg));
        
        // Convert address to hex (simplified)
        uintptr_t addr = reinterpret_cast<uintptr_t>(info->si_addr);
        char hex[32];
        len = snprintf(hex, sizeof(hex), "0x%lx", addr);
        write(STDERR_FILENO, hex, len);
        write(STDERR_FILENO, "\n", 1);
    }
    
    // Generate stack trace
    void* buffer[100];
    int frames = backtrace(buffer, 100);
    backtrace_symbols_fd(buffer, frames, STDERR_FILENO);
    
    // Don't reinstall - sigaction persists
}

/**
 * Safe crash logger class
 */
class CrashLogger {
private:
    static inline bool initialized = false;
    static inline char crash_message[256];
    
public:
    static void initialize() {
        if (!initialized) {
            // Pre-format crash message (safe because it's before signals)
            strcpy(crash_message, "Crash Logger: Fatal signal ");
            
            // Set up signal handlers using sigaction
            struct sigaction sa;
            sa.sa_sigaction = safer_signal_handler;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = SA_SIGINFO | SA_RESTART;
            
            // Handle common crash signals
            sigaction(SIGSEGV, &sa, nullptr);  // Segmentation fault
            sigaction(SIGABRT, &sa, nullptr);  // Abort
            sigaction(SIGFPE, &sa, nullptr);   // Floating point exception
            sigaction(SIGILL, &sa, nullptr);    // Illegal instruction
            sigaction(SIGBUS, &sa, nullptr);    // Bus error
            
            initialized = true;
            
            // Safe write during initialization
            const char* init_msg = "Crash logger initialized\n";
            write(STDERR_FILENO, init_msg, strlen(init_msg));
        }
    }
};

/**
 * Function that will crash
 */
void cause_crash(int type) {
    switch (type) {
        case 1: {
            // Segmentation fault
            int* p = nullptr;
            *p = 42;
            break;
        }
        case 2: {
            // Division by zero
            int a = 10, b = 0;
            int c = a / b;
            (void)c;
            break;
        }
        case 3: {
            // Abort
            abort();
            break;
        }
        default: {
            // Infinite loop (won't crash)
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
}

/**
 * Demonstrate dangerous signal handlers
 */
void demonstrate_dangerous() {
    std::cout << "\n=== DANGEROUS Signal Handler (will likely crash) ===\n";
    std::cout << "WARNING: This will demonstrate undefined behavior!\n";
    std::cout << "Press Enter to install dangerous handler...\n";
    std::cin.get();
    
    // Install dangerous handler
    signal(SIGSEGV, dangerous_signal_handler);
    signal(SIGABRT, dangerous_signal_handler);
    
    std::cout << "Dangerous handler installed. Crashing in 2 seconds...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // This will likely crash in the signal handler itself
    cause_crash(1);  // Segmentation fault
}

/**
 * Demonstrate safe signal handlers
 */
void demonstrate_safe() {
    std::cout << "\n=== SAFE Signal Handler ===\n";
    
    // Initialize safe crash logger
    CrashLogger::initialize();
    
    std::cout << "Safe handler installed. Crashing in 2 seconds...\n";
    std::cout << "Check stderr for crash log\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    cause_crash(1);  // Segmentation fault
}

/**
 * Demonstrate different crash types
 */
void demonstrate_crash_types() {
    std::cout << "\n=== Different Crash Types ===\n";
    
    CrashLogger::initialize();
    
    std::cout << "Choose crash type:\n";
    std::cout << "1: Segmentation fault\n";
    std::cout << "2: Division by zero\n";
    std::cout << "3: Abort\n";
    std::cout << "> ";
    
    int choice;
    std::cin >> choice;
    
    cause_crash(choice);
}

/**
 * Main function
 */
int main(int argc, char* argv[]) {
    std::cout << "========================================\n";
    std::cout << "Signal Handler Crash Logger\n";
    std::cout << "========================================\n\n";
    
    std::cout << "Async-Signal-Safe Functions:\n";
    std::cout << "✓ write()\n";
    std::cout << "✓ _exit()\n";
    std::cout << "✓ sigaction()\n";
    std::cout << "✓ backtrace() (usually safe)\n";
    std::cout << "✓ open()/close()/read()/write() (carefully)\n\n";
    
    std::cout << "NOT Async-Signal-Safe:\n";
    std::cout << "✗ malloc()/free()\n";
    std::cout << "✗ new/delete\n";
    std::cout << "✗ std::cout/cerr\n";
    std::cout << "✗ sprintf()\n";
    std::cout << "✗ exit()\n";
    std::cout << "✗ Most C++ standard library functions\n\n";
    
    if (argc > 1 && strcmp(argv[1], "--safe") == 0) {
        demonstrate_safe();
    } else if (argc > 1 && strcmp(argv[1], "--dangerous") == 0) {
        demonstrate_dangerous();
    } else if (argc > 1 && strcmp(argv[1], "--crash") == 0) {
        demonstrate_crash_types();
    } else {
        std::cout << "Usage: " << argv[0] << " [--safe|--dangerous|--crash]\n";
        std::cout << "\nRunning safe demonstration by default...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        demonstrate_safe();
    }
    
    return 0;
}