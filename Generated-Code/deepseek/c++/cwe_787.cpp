#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>

/**
 * Custom exception for buffer overflow errors
 */
class BufferOverflowError : public std::runtime_error {
public:
    explicit BufferOverflowError(const std::string& message) 
        : std::runtime_error("Buffer Overflow Error: " + message) {}
};

/**
 * Memory tracker to monitor buffer operations
 */
class BufferTracker {
private:
    static inline size_t total_reads = 0;
    static inline size_t overflow_attempts = 0;
    
public:
    static void record_read(size_t bytes_read, size_t buffer_size) {
        total_reads++;
        if (bytes_read > buffer_size) {
            overflow_attempts++;
            std::cout << "[TRACKER] ⚠️  Buffer overflow attempt detected! "
                      << "Reading " << bytes_read << " bytes into "
                      << buffer_size << " byte buffer\n";
        }
    }
    
    static void print_stats() {
        std::cout << "\n=== Buffer Statistics ===\n";
        std::cout << "Total read operations: " << total_reads << "\n";
        std::cout << "Overflow attempts: " << overflow_attempts << "\n";
    }
};

/**
 * DANGEROUS: Reads 100 bytes into 50-byte buffer
 * THIS IS UNSAFE - FOR DEMONSTRATION ONLY
 */
void dangerousFileRead(const std::string& filename) {
    std::cout << "\n=== DANGEROUS: Reading 100 bytes into 50-byte buffer ===\n";
    std::cout << "WARNING: This demonstrates buffer overflow!\n\n";
    
    // 50-byte stack buffer
    char buffer[50];
    
    std::cout << "Buffer address: " << static_cast<void*>(buffer) << "\n";
    std::cout << "Buffer size: 50 bytes\n";
    std::cout << "Attempting to read 100 bytes...\n";
    
    // Open file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }
    
    // DANGEROUS: Reading 100 bytes into 50-byte buffer
    file.read(buffer, 100);  // Buffer overflow!
    
    // Get actual bytes read
    std::streamsize bytes_read = file.gcount();
    BufferTracker::record_read(bytes_read, sizeof(buffer));
    
    std::cout << "File read attempted. Bytes actually read: " << bytes_read << "\n";
    std::cout << "Buffer now contains: ";
    for (int i = 0; i < std::min<int>(bytes_read, 50); ++i) {
        if (isprint(buffer[i])) {
            std::cout << buffer[i];
        } else {
            std::cout << "[" << std::hex << (int)buffer[i] << std::dec << "]";
        }
    }
    std::cout << "\n";
    
    if (bytes_read > 50) {
        std::cout << "\n⚠️  BUFFER OVERFLOW OCCURRED!\n";
        std::cout << "Memory beyond the buffer has been corrupted!\n";
        
        // Show corruption by looking at stack variables
        int canary1 = 0xDEADBEEF;
        int canary2 = 0xCAFEBABE;
        
        std::cout << "\nStack canaries before overflow:\n";
        std::cout << "  canary1: 0x" << std::hex << canary1 << std::dec << "\n";
        std::cout << "  canary2: 0x" << std::hex << canary2 << std::dec << "\n";
        
        // The overflow may have corrupted these if buffer is on stack
        std::cout << "\nNote: The overflow may have corrupted adjacent stack variables!\n";
    }
    
    file.close();
}

/**
 * DANGEROUS: Another example with string data
 */
void dangerousStringRead(const std::string& filename) {
    std::cout << "\n=== DANGEROUS: Reading string data ===\n";
    
    char buffer[50];
    
    std::ifstream file(filename);
    if (!file.is_open()) return;
    
    // DANGEROUS: No bounds checking
    file.getline(buffer, 100);  // Specifying 100 but buffer only 50!
    
    std::cout << "Read line (unsafe): " << buffer << "\n";
    std::cout << "This may overflow if line > 49 characters\n";
    
    file.close();
}

/**
 * SAFE: Read with bounds checking
 */
void safeFileRead(const std::string& filename) {
    std::cout << "\n=== SAFE: Reading with bounds checking ===\n";
    
    char buffer[50];
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }
    
    // SAFE: Only read up to buffer size
    file.read(buffer, sizeof(buffer) - 1);  // Leave room for null terminator
    std::streamsize bytes_read = file.gcount();
    buffer[bytes_read] = '\0';  // Null terminate
    
    BufferTracker::record_read(bytes_read, sizeof(buffer));
    
    std::cout << "Safely read " << bytes_read << " bytes\n";
    std::cout << "Buffer contents: " << buffer << "\n";
    
    file.close();
}

/**
 * SAFE: Using getline with size limit
 */
void safeGetlineRead(const std::string& filename) {
    std::cout << "\n=== SAFE: Using getline with size limit ===\n";
    
    char buffer[50];
    
    std::ifstream file(filename);
    if (!file.is_open()) return;
    
    // SAFE: Specify actual buffer size
    file.getline(buffer, sizeof(buffer));
    
    std::cout << "Read line (safe): " << buffer << "\n";
    std::cout << "Input was truncated if longer than " << sizeof(buffer) - 1 << " chars\n";
    
    file.close();
}

/**
 * SAFE: Using std::string (no overflow risk)
 */
void safeStringRead(const std::string& filename) {
    std::cout << "\n=== SAFE: Using std::string ===\n";
    
    std::ifstream file(filename);
    if (!file.is_open()) return;
    
    std::string content;
    std::string line;
    
    // SAFE: std::string grows as needed
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    std::cout << "Read " << content.length() << " characters using std::string\n";
    std::cout << "First 50 chars: " << content.substr(0, 50) << "\n";
    
    file.close();
}

/**
 * SAFE: Using vector for binary data
 */
void safeVectorRead(const std::string& filename) {
    std::cout << "\n=== SAFE: Using std::vector ===\n";
    
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return;
    
    // Get file size
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // SAFE: Vector allocates exactly what's needed
    std::vector<char> buffer(size);
    
    if (file.read(buffer.data(), size)) {
        std::cout << "Read " << size << " bytes into vector\n";
        std::cout << "First 50 bytes: ";
        for (int i = 0; i < std::min<size_t>(50, buffer.size()); ++i) {
            if (isprint(buffer[i])) {
                std::cout << buffer[i];
            } else {
                std::cout << "[" << std::hex << (int)buffer[i] << std::dec << "]";
            }
        }
        std::cout << "\n";
    }
    
    file.close();
}

/**
 * Create a test file with 100+ bytes of data
 */
void createTestFile(const std::string& filename) {
    std::ofstream file(filename);
    
    // Write more than 100 bytes
    file << "This is a test file with more than 100 bytes of data. ";
    file << "It contains exactly 100+ characters to demonstrate buffer overflow. ";
    file << "The quick brown fox jumps over the lazy dog. ";
    file << "1234567890 1234567890 1234567890 1234567890.";
    
    file.close();
    std::cout << "Created test file: " << filename << "\n";
}

/**
 * Demonstrate stack corruption
 */
void demonstrateStackCorruption() {
    std::cout << "\n=== Stack Corruption Demonstration ===\n";
    
    // Place canaries around the buffer
    volatile uint32_t canary_before = 0xDEADBEEF;
    char buffer[50];
    volatile uint32_t canary_after = 0xCAFEBABE;
    
    std::cout << "Stack layout:\n";
    std::cout << "  canary_before at: " << static_cast<void*>(const_cast<uint32_t*>(&canary_before)) << "\n";
    std::cout << "  buffer at: " << static_cast<void*>(buffer) << "\n";
    std::cout << "  canary_after at: " << static_cast<void*>(const_cast<uint32_t*>(&canary_after)) << "\n";
    std::cout << "  canary_before value: 0x" << std::hex << canary_before << std::dec << "\n";
    std::cout << "  canary_after value: 0x" << std::hex << canary_after << std::dec << "\n\n";
    
    std::cout << "Simulating overflow by writing 60 bytes...\n";
    
    // Simulate overflow (for demonstration)
    for (int i = 0; i < 60; ++i) {
        buffer[i] = 'A' + (i % 26);
    }
    
    std::cout << "After overflow:\n";
    std::cout << "  canary_before value: 0x" << std::hex << canary_before << std::dec << "\n";
    std::cout << "  canary_after value: 0x" << std::hex << canary_after << std::dec << "\n";
    
    if (canary_before != 0xDEADBEEF || canary_after != 0xCAFEBABE) {
        std::cout << "⚠️  Canaries corrupted! Stack corruption detected!\n";
    }
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Buffer Overflow: 100 bytes into 50-byte buffer\n";
    std::cout << "========================================\n";
    
    const std::string filename = "test_data.txt";
    
    // Create test file
    createTestFile(filename);
    
    try {
        // Show the dangerous way
        std::cout << "\nPress Enter to see DANGEROUS example...\n";
        std::cin.get();
        
        dangerousFileRead(filename);
        dangerousStringRead(filename);
        
        // Demonstrate stack corruption
        demonstrateStackCorruption();
        
        // Show safe alternatives
        std::cout << "\n\nPress Enter to see SAFE alternatives...\n";
        std::cin.get();
        
        safeFileRead(filename);
        safeGetlineRead(filename);
        safeStringRead(filename);
        safeVectorRead(filename);
        
        // Statistics
        BufferTracker::print_stats();
        
        // Summary
        std::cout << "\n=== Summary ===\n";
        std::cout << "❌ NEVER read more data than buffer can hold\n";
        std::cout << "✓ Always check buffer size before reading\n";
        std::cout << "✓ Use std::string for text data\n";
        std::cout << "✓ Use std::vector for binary data\n";
        std::cout << "✓ Specify buffer size in getline()\n";
        std::cout << "✓ Check gcount() after read operations\n";
        
        std::cout << "\n=== Consequences of Buffer Overflow ===\n";
        std::cout << "1. Program crash\n";
        std::cout << "2. Memory corruption\n";
        std::cout << "3. Security vulnerabilities\n";
        std::cout << "4. Undefined behavior\n";
        std::cout << "5. Hard-to-debug issues\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}