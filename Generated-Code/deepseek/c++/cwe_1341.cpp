#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>
#include <stdexcept>

// Platform-specific socket includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

/**
 * Custom exception for socket errors
 */
class SocketError : public std::runtime_error {
public:
    explicit SocketError(const std::string& message) 
        : std::runtime_error("Socket Error: " + message) {}
};

/**
 * Socket tracker for debugging
 */
class SocketTracker {
private:
    static inline std::map<SOCKET, std::string> open_sockets;
    static inline std::mutex tracker_mutex;
    
public:
    static void record_open(SOCKET sock, const std::string& description) {
        std::lock_guard<std::mutex> lock(tracker_mutex);
        open_sockets[sock] = description;
        std::cout << "[SOCKET " << sock << "] Opened: " << description << "\n";
    }
    
    static void record_close(SOCKET sock) {
        std::lock_guard<std::mutex> lock(tracker_mutex);
        auto it = open_sockets.find(sock);
        if (it != open_sockets.end()) {
            std::cout << "[SOCKET " << sock << "] Closed: " << it->second << "\n";
            open_sockets.erase(it);
        } else {
            std::cout << "[SOCKET " << sock << "] Attempted to close unknown socket\n";
        }
    }
    
    static void print_status() {
        std::lock_guard<std::mutex> lock(tracker_mutex);
        std::cout << "\n=== Open Sockets ===\n";
        if (open_sockets.empty()) {
            std::cout << "No open sockets\n";
        } else {
            for (const auto& [sock, desc] : open_sockets) {
                std::cout << "  Socket " << sock << ": " << desc << "\n";
            }
        }
    }
};

/**
 * RAII socket wrapper for automatic cleanup
 */
class RAIIWrapper {
private:
    SOCKET sock;
    bool owns_socket;
    
public:
    RAIIWrapper() : sock(INVALID_SOCKET), owns_socket(false) {}
    
    explicit RAIIWrapper(SOCKET s, bool take_ownership = true) 
        : sock(s), owns_socket(take_ownership) {
        if (s != INVALID_SOCKET && owns_socket) {
            SocketTracker::record_open(s, "RAII managed");
        }
    }
    
    ~RAIIWrapper() {
        cleanup();
    }
    
    // Disable copying
    RAIIWrapper(const RAIIWrapper&) = delete;
    RAIIWrapper& operator=(const RAIIWrapper&) = delete;
    
    // Enable moving
    RAIIWrapper(RAIIWrapper&& other) noexcept
        : sock(other.sock), owns_socket(other.owns_socket) {
        other.sock = INVALID_SOCKET;
        other.owns_socket = false;
    }
    
    void cleanup() {
        if (owns_socket && sock != INVALID_SOCKET) {
            closesocket(sock);
            SocketTracker::record_close(sock);
            sock = INVALID_SOCKET;
            owns_socket = false;
        }
    }
    
    void reset(SOCKET new_sock = INVALID_SOCKET, bool take_ownership = true) {
        cleanup();
        sock = new_sock;
        owns_socket = take_ownership;
        if (sock != INVALID_SOCKET && owns_socket) {
            SocketTracker::record_open(sock, "RAII reset");
        }
    }
    
    SOCKET get() const { return sock; }
    bool isValid() const { return sock != INVALID_SOCKET; }
};

/**
 * Socket cleanup function - closes socket in both try and catch blocks
 */
void socketCleanupExample(const std::string& host, int port) {
    std::cout << "\n=== Socket Cleanup Example ===\n";
    
    SOCKET sock = INVALID_SOCKET;
    
    try {
        std::cout << "Attempting to create socket...\n";
        
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            throw SocketError("Failed to create socket");
        }
        
        SocketTracker::record_open(sock, "Main socket");
        
        // Set up server address
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        
        // Convert IP address
        if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
            throw SocketError("Invalid address or address not supported");
        }
        
        std::cout << "Connecting to " << host << ":" << port << "...\n";
        
        // Connect (may throw)
        if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            throw SocketError("Connection failed");
        }
        
        std::cout << "Connected successfully!\n";
        
        // Simulate some work
        std::cout << "Performing network operations...\n";
        
        // This might throw (simulate random failure)
        if (rand() % 3 == 0) {
            throw SocketError("Simulated network error during operation");
        }
        
        std::cout << "Operations completed successfully.\n";
        
        // Cleanup in try block
        std::cout << "Cleaning up socket in try block...\n";
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            SocketTracker::record_close(sock);
            sock = INVALID_SOCKET;
        }
        
    } catch (const std::exception& e) {
        std::cout << "\nException caught: " << e.what() << "\n";
        
        // Cleanup in catch block (same as try block)
        std::cout << "Cleaning up socket in catch block...\n";
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            SocketTracker::record_close(sock);
            sock = INVALID_SOCKET;
        }
        
        // Re-throw or handle as needed
        throw;  // Re-throw for demonstration
    }
}

/**
 * Multiple socket cleanup example
 */
void multipleSocketCleanup() {
    std::cout << "\n=== Multiple Socket Cleanup ===\n";
    
    SOCKET sock1 = INVALID_SOCKET;
    SOCKET sock2 = INVALID_SOCKET;
    
    try {
        // Create first socket
        sock1 = socket(AF_INET, SOCK_STREAM, 0);
        if (sock1 == INVALID_SOCKET) throw SocketError("Failed to create socket 1");
        SocketTracker::record_open(sock1, "Socket 1");
        
        // Create second socket
        sock2 = socket(AF_INET, SOCK_STREAM, 0);
        if (sock2 == INVALID_SOCKET) throw SocketError("Failed to create socket 2");
        SocketTracker::record_open(sock2, "Socket 2");
        
        std::cout << "Both sockets created successfully\n";
        
        // Simulate work that might fail
        if (rand() % 2 == 0) {
            throw SocketError("Simulated failure during work");
        }
        
        std::cout << "Work completed successfully\n";
        
        // Cleanup in try block
        std::cout << "Cleaning up in try block...\n";
        if (sock1 != INVALID_SOCKET) {
            closesocket(sock1);
            SocketTracker::record_close(sock1);
            sock1 = INVALID_SOCKET;
        }
        if (sock2 != INVALID_SOCKET) {
            closesocket(sock2);
            SocketTracker::record_close(sock2);
            sock2 = INVALID_SOCKET;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
        
        // Cleanup in catch block
        std::cout << "Cleaning up in catch block...\n";
        if (sock1 != INVALID_SOCKET) {
            closesocket(sock1);
            SocketTracker::record_close(sock1);
            sock1 = INVALID_SOCKET;
        }
        if (sock2 != INVALID_SOCKET) {
            closesocket(sock2);
            SocketTracker::record_close(sock2);
            sock2 = INVALID_SOCKET;
        }
        
        throw;
    }
}

/**
 * RAII-based approach (cleaner than manual try/catch cleanup)
 */
void raiiCleanupExample(const std::string& host, int port) {
    std::cout << "\n=== RAII Cleanup Example ===\n";
    
    RAIIWrapper socket_wrapper;
    
    try {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            throw SocketError("Failed to create socket");
        }
        
        socket_wrapper.reset(sock, true);  // RAII now manages socket
        
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        
        if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
            throw SocketError("Invalid address");
        }
        
        if (connect(socket_wrapper.get(), (struct sockaddr*)&server_addr, 
                    sizeof(server_addr)) < 0) {
            throw SocketError("Connection failed");
        }
        
        std::cout << "Connected successfully!\n";
        
        // Simulate work that might fail
        if (rand() % 3 == 0) {
            throw SocketError("Simulated error during work");
        }
        
        std::cout << "Work completed successfully.\n";
        
        // Socket will be automatically cleaned up by RAII wrapper
        std::cout << "Socket will be auto-closed by RAII destructor\n";
        
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << "\n";
        // Socket still automatically cleaned up by RAII
        throw;
    }
}

/**
 * Nested try-catch with multiple resources
 */
void nestedCleanupExample() {
    std::cout << "\n=== Nested Try-Catch Cleanup ===\n";
    
    SOCKET sock = INVALID_SOCKET;
    
    try {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) throw SocketError("Failed to create socket");
        SocketTracker::record_open(sock, "Nested example socket");
        
        try {
            // Inner try block
            std::cout << "Inner try block\n";
            
            if (rand() % 2 == 0) {
                throw SocketError("Error in inner block");
            }
            
            std::cout << "Inner block succeeded\n";
            
        } catch (const std::exception& e) {
            std::cout << "Inner catch: " << e.what() << "\n";
            
            // Cleanup in inner catch
            if (sock != INVALID_SOCKET) {
                closesocket(sock);
                SocketTracker::record_close(sock);
                sock = INVALID_SOCKET;
            }
            
            throw;  // Re-throw to outer catch
        }
        
        // Cleanup in outer try
        std::cout << "Cleaning up in outer try\n";
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            SocketTracker::record_close(sock);
            sock = INVALID_SOCKET;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Outer catch: " << e.what() << "\n";
        
        // Cleanup in outer catch (in case inner didn't clean up)
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            SocketTracker::record_close(sock);
            sock = INVALID_SOCKET;
        }
    }
}

/**
 * Function that demonstrates the core requirement: cleanup in both try and catch
 */
void coreCleanupExample() {
    std::cout << "\n=== CORE EXAMPLE: Cleanup in Try and Catch ===\n";
    
    SOCKET sock = INVALID_SOCKET;
    
    try {
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            throw SocketError("Socket creation failed");
        }
        
        SocketTracker::record_open(sock, "Core example");
        
        std::cout << "Socket created successfully\n";
        
        // Simulate some operation that might fail
        std::cout << "Performing operation...\n";
        
        if (rand() % 2 == 0) {
            throw SocketError("Operation failed!");
        }
        
        std::cout << "Operation succeeded!\n";
        
        // CLEANUP IN TRY BLOCK
        std::cout << "Cleaning up socket in TRY block\n";
        closesocket(sock);
        SocketTracker::record_close(sock);
        sock = INVALID_SOCKET;
        
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << "\n";
        
        // CLEANUP IN CATCH BLOCK (same as try block)
        std::cout << "Cleaning up socket in CATCH block\n";
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
            SocketTracker::record_close(sock);
            sock = INVALID_SOCKET;
        }
        
        // Handle or re-throw as needed
        std::cout << "Exception handled, socket cleaned up\n";
    }
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Socket Cleanup in Try and Catch Blocks\n";
    std::cout << "========================================\n";
    
    // Initialize random seed
    srand(time(nullptr));
    
#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
    std::cout << "Winsock initialized\n";
#endif
    
    try {
        // Core example (as requested)
        for (int i = 0; i < 3; ++i) {
            coreCleanupExample();
        }
        
        // More comprehensive examples
        try {
            socketCleanupExample("127.0.0.1", 8080);
        } catch (const std::exception& e) {
            std::cout << "Main caught: " << e.what() << "\n";
        }
        
        try {
            multipleSocketCleanup();
        } catch (const std::exception& e) {
            std::cout << "Main caught multiple socket error\n";
        }
        
        try {
            raiiCleanupExample("127.0.0.1", 9090);
        } catch (const std::exception& e) {
            std::cout << "RAII example exception handled\n";
        }
        
        nestedCleanupExample();
        
        // Show final socket status
        SocketTracker::print_status();
        
        // Summary
        std::cout << "\n=== Key Takeaways ===\n";
        std::cout << "✓ Close sockets in try block on success\n";
        std::cout << "✓ Close sockets in catch block on failure\n";
        std::cout << "✓ Check socket validity before closing\n";
        std::cout << "✓ Set socket to INVALID_SOCKET after closing\n";
        std::cout << "✓ Consider RAII for automatic cleanup\n";
        std::cout << "✓ Always cleanup resources regardless of path\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
    }
    
#ifdef _WIN32
    WSACleanup();
    std::cout << "Winsock cleaned up\n";
#endif
    
    return 0;
}