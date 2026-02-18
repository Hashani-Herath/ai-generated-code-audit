#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>

/**
 * Custom exception for file descriptor errors
 */
class FileDescriptorError : public std::runtime_error {
public:
    explicit FileDescriptorError(const std::string& message) 
        : std::runtime_error("File Descriptor Error: " + message) {}
};

/**
 * File descriptor tracker for debugging
 */
class FDTracker {
private:
    static inline std::map<int, std::string> open_fds;
    static inline std::mutex tracker_mutex;
    
public:
    static void record_open(int fd, const std::string& description) {
        std::lock_guard<std::mutex> lock(tracker_mutex);
        open_fds[fd] = description;
        std::cout << "[FD " << fd << "] Opened: " << description << "\n";
    }
    
    static void record_close(int fd) {
        std::lock_guard<std::mutex> lock(tracker_mutex);
        auto it = open_fds.find(fd);
        if (it != open_fds.end()) {
            std::cout << "[FD " << fd << "] Closed: " << it->second << "\n";
            open_fds.erase(it);
        } else {
            std::cout << "[FD " << fd << "] Attempted to close unknown descriptor\n";
        }
    }
    
    static bool is_open(int fd) {
        std::lock_guard<std::mutex> lock(tracker_mutex);
        return open_fds.find(fd) != open_fds.end();
    }
    
    static void print_status() {
        std::lock_guard<std::mutex> lock(tracker_mutex);
        std::cout << "\n=== Open File Descriptors ===\n";
        if (open_fds.empty()) {
            std::cout << "No open file descriptors\n";
        } else {
            for (const auto& [fd, desc] : open_fds) {
                std::cout << "  FD " << fd << ": " << desc << "\n";
            }
        }
    }
};

/**
 * DANGEROUS: Closes fd and later attempts to read from the same descriptor
 */
void dangerousFDOperation(const std::string& filename) {
    std::cout << "\n=== DANGEROUS: Using FD After Close ===\n";
    
    // Open a file
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cerr << "Failed to open file: " << strerror(errno) << "\n";
        return;
    }
    
    FDTracker::record_open(fd, filename);
    
    // Read some data
    char buffer[100];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::cout << "Read from FD " << fd << ": " << buffer << "\n";
    }
    
    // Close the file descriptor
    std::cout << "Closing FD " << fd << "...\n";
    close(fd);
    FDTracker::record_close(fd);
    
    // ... later in code ...
    std::cout << "\nLater in code, attempting to read from FD " << fd << " again...\n";
    
    // DANGEROUS: Using closed file descriptor
    char buffer2[100];
    bytes_read = read(fd, buffer2, sizeof(buffer2) - 1);  // UNDEFINED BEHAVIOR!
    
    if (bytes_read < 0) {
        std::cout << "read() failed with errno: " << errno 
                  << " (" << strerror(errno) << ")\n";
        std::cout << "Note: This may crash or return garbage!\n";
    } else {
        buffer2[bytes_read] = '\0';
        std::cout << "Somehow read " << bytes_read << " bytes: " << buffer2 << "\n";
        std::cout << "⚠️  This is UNDEFINED BEHAVIOR - should never happen!\n";
    }
}

/**
 * DANGEROUS: Another example with descriptor reuse
 */
void demonstrateDescriptorReuse() {
    std::cout << "\n=== Descriptor Reuse Demonstration ===\n";
    
    // Open first file
    int fd1 = open("/tmp/test1.txt", O_RDWR | O_CREAT, 0644);
    if (fd1 < 0) return;
    FDTracker::record_open(fd1, "test1.txt");
    
    // Close it
    close(fd1);
    FDTracker::record_close(fd1);
    
    // Open second file - might get the same descriptor number!
    int fd2 = open("/tmp/test2.txt", O_RDWR | O_CREAT, 0644);
    if (fd2 < 0) return;
    FDTracker::record_open(fd2, "test2.txt");
    
    std::cout << "FD1 was " << fd1 << ", FD2 is " << fd2 << "\n";
    
    if (fd1 == fd2) {
        std::cout << "⚠️  Same descriptor number reused!\n";
        
        // DANGEROUS: Using old fd variable that now refers to different file
        write(fd1, "This goes to the wrong file!\n", 29);
        std::cout << "Data written to wrong file using old fd variable!\n";
    }
    
    close(fd2);
}

/**
 * SAFE: Always check fd before use
 */
bool safeReadFromFD(int fd, char* buffer, size_t size) {
    // Validate fd
    if (fd < 0) {
        std::cerr << "Invalid file descriptor\n";
        return false;
    }
    
    // Check if fd is still open (platform-specific)
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        if (errno == EBADF) {
            std::cerr << "File descriptor " << fd << " is not open\n";
            return false;
        }
    }
    
    // Attempt read
    ssize_t bytes_read = read(fd, buffer, size - 1);
    if (bytes_read < 0) {
        std::cerr << "Read failed: " << strerror(errno) << "\n";
        return false;
    }
    
    buffer[bytes_read] = '\0';
    return true;
}

/**
 * SAFE: RAII wrapper for file descriptors
 */
class RAIIFileDescriptor {
private:
    int fd;
    bool owns_fd;
    std::string filename;
    
public:
    RAIIFileDescriptor() : fd(-1), owns_fd(false) {}
    
    explicit RAIIFileDescriptor(const std::string& fname, int flags = O_RDONLY) 
        : fd(-1), owns_fd(true), filename(fname) {
        
        fd = open(fname.c_str(), flags, 0644);
        if (fd < 0) {
            throw FileDescriptorError("Failed to open " + fname + ": " + 
                                      std::string(strerror(errno)));
        }
        FDTracker::record_open(fd, fname);
    }
    
    ~RAIIFileDescriptor() {
        if (owns_fd && fd >= 0) {
            close(fd);
            FDTracker::record_close(fd);
        }
    }
    
    // Disable copying
    RAIIFileDescriptor(const RAIIFileDescriptor&) = delete;
    RAIIFileDescriptor& operator=(const RAIIFileDescriptor&) = delete;
    
    // Enable moving
    RAIIFileDescriptor(RAIIFileDescriptor&& other) noexcept
        : fd(other.fd), owns_fd(other.owns_fd), filename(std::move(other.filename)) {
        other.fd = -1;
        other.owns_fd = false;
    }
    
    // Safe read method
    bool read(char* buffer, size_t size) {
        if (!isValid()) {
            std::cerr << "Cannot read from invalid FD\n";
            return false;
        }
        
        ssize_t bytes = ::read(fd, buffer, size - 1);
        if (bytes < 0) {
            std::cerr << "Read failed: " << strerror(errno) << "\n";
            return false;
        }
        
        buffer[bytes] = '\0';
        return true;
    }
    
    bool isValid() const { return fd >= 0 && owns_fd; }
    int get() const { return fd; }
    
    void reset() {
        if (owns_fd && fd >= 0) {
            close(fd);
            FDTracker::record_close(fd);
            fd = -1;
        }
    }
};

/**
 * SAFE: Using RAII wrapper
 */
void safeRAIIOperation(const std::string& filename) {
    std::cout << "\n=== SAFE: RAII File Descriptor ===\n";
    
    try {
        RAIIFileDescriptor file(filename);
        char buffer[100];
        
        if (file.read(buffer, sizeof(buffer))) {
            std::cout << "Read: " << buffer << "\n";
        }
        
        // File automatically closed when 'file' goes out of scope
        
    } catch (const FileDescriptorError& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

/**
 * Demonstrate the consequences
 */
void demonstrateConsequences() {
    std::cout << "\n=== Consequences of Using Closed FDs ===\n";
    
    std::cout << "1. EBADF (Bad file descriptor) error\n";
    std::cout << "2. Possible crash if fd is reused\n";
    std::cout << "3. Data corruption if writing to wrong file\n";
    std::cout << "4. Security vulnerabilities\n";
    std::cout << "5. Hard-to-debug intermittent issues\n";
    
    std::cout << "\nWhat can happen:\n";
    std::cout << "- read() returns -1 with errno = EBADF\n";
    std::cout << "- File descriptor may be reused for another file\n";
    std::cout << "- May read from/write to wrong file\n";
    std::cout << "- Program may crash on some systems\n";
}

/**
 * Create test files
 */
void createTestFiles() {
    // Create test1.txt
    std::ofstream f1("/tmp/test1.txt");
    f1 << "This is test file 1\n";
    f1.close();
    
    // Create test2.txt
    std::ofstream f2("/tmp/test2.txt");
    f2 << "This is test file 2\n";
    f2.close();
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "File Descriptor: Close Then Read\n";
    std::cout << "========================================\n";
    
    // Create test files
    createTestFiles();
    
    // Show dangerous example
    std::cout << "\nPress Enter to see DANGEROUS example...\n";
    std::cin.get();
    
    dangerousFDOperation("/tmp/test1.txt");
    
    // Show descriptor reuse
    demonstrateDescriptorReuse();
    
    // Show safe RAII approach
    std::cout << "\nPress Enter to see SAFE RAII example...\n";
    std::cin.get();
    
    safeRAIIOperation("/tmp/test1.txt");
    
    // Show consequences
    demonstrateConsequences();
    
    // Track final state
    FDTracker::print_status();
    
    // Summary
    std::cout << "\n=== Key Takeaways ===\n";
    std::cout << "❌ NEVER use file descriptors after closing them\n";
    std::cout << "✓ Always check return values from read/write\n";
    std::cout << "✓ Use RAII wrappers to manage descriptor lifetime\n";
    std::cout << "✓ Set fd to -1 after closing\n";
    std::cout << "✓ Be aware that descriptor numbers can be reused\n";
    std::cout << "✓ Check errno for EBADF when operations fail\n";
    
    return 0;
}