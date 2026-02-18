#include <iostream>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstdint>

/**
 * Custom exception for packet handling errors
 */
class PacketError : public std::runtime_error {
public:
    explicit PacketError(const std::string& message) 
        : std::runtime_error("Packet Error: " + message) {}
};

/**
 * Result structure for packet copy operation
 */
struct PacketCopyResult {
    bool success;
    size_t bytes_copied;
    std::string error_message;
    void* dest_ptr;
    const void* src_ptr;
    
    PacketCopyResult() : success(false), bytes_copied(0), dest_ptr(nullptr), src_ptr(nullptr) {}
};

/**
 * Security levels for packet validation
 */
enum class SecurityLevel {
    RELAXED,    // Basic bounds checking only
    STRICT,     // Additional sanity checks
    PARANOID    // Maximum security checks
};

/**
 * Configuration for packet copier
 */
struct PacketCopierConfig {
    size_t max_packet_size;
    size_t min_packet_size;
    SecurityLevel security_level;
    bool allow_zero_length;
    bool check_dest_capacity;
    bool validate_pointers;
    bool log_operations;
    
    PacketCopierConfig() 
        : max_packet_size(1024 * 1024)  // 1 MB default max
        , min_packet_size(0)
        , security_level(SecurityLevel::STRICT)
        , allow_zero_length(false)
        , check_dest_capacity(true)
        , validate_pointers(true)
        , log_operations(false) {}
};

/**
 * Main packet copier class
 */
class SafePacketCopier {
private:
    PacketCopierConfig config;
    
    /**
     * Validate source and destination pointers
     */
    bool validatePointers(void* dest, const void* src, size_t length) const {
        if (!config.validate_pointers) return true;
        
        if (dest == nullptr || src == nullptr) {
            return false;
        }
        
        // Check for overlap (important for memcpy vs memmove)
        const char* dest_char = static_cast<char*>(dest);
        const char* src_char = static_cast<const char*>(src);
        
        // Check if ranges overlap
        if ((dest_char <= src_char && dest_char + length > src_char) ||
            (src_char <= dest_char && src_char + length > dest_char)) {
            return false;  // Overlapping ranges should use memmove
        }
        
        return true;
    }
    
    /**
     * Log operation if enabled
     */
    void logOperation(const std::string& operation, int signed_length, 
                     size_t unsigned_length) const {
        if (config.log_operations) {
            std::cout << "[PacketCopier] " << operation 
                      << " | Signed length: " << signed_length
                      << " | Unsafe length: " << unsigned_length
                      << " | Safe length: " << unsigned_length << "\n";
        }
    }
    
    /**
     * Sanity check the packet length based on security level
     */
    bool sanityCheck(int signed_length, size_t unsigned_length) const {
        switch (config.security_level) {
            case SecurityLevel::PARANOID:
                // Paranoid: Check for suspicious values
                if (signed_length == 0xDEADBEEF || 
                    signed_length == 0xFFFFFFFF ||
                    signed_length == std::numeric_limits<int>::max() ||
                    signed_length == std::numeric_limits<int>::min()) {
                    return false;
                }
                // Fall through to STRICT
                
            case SecurityLevel::STRICT:
                // Strict: Check for power of two (common in attacks)
                if (unsigned_length > 0 && 
                    (unsigned_length & (unsigned_length - 1)) == 0) {
                    // Suspicious but not necessarily invalid
                    if (unsigned_length > 1024) return false;
                }
                // Check for common attack patterns
                if (unsigned_length > config.max_packet_size / 2 &&
                    unsigned_length < config.max_packet_size) {
                    // Near maximum - could be DoS attempt
                    if (config.security_level == SecurityLevel::PARANOID) return false;
                }
                break;
                
            case SecurityLevel::RELAXED:
            default:
                // Relaxed: No extra checks
                break;
        }
        
        return true;
    }
    
public:
    explicit SafePacketCopier(const PacketCopierConfig& cfg = PacketCopierConfig{}) 
        : config(cfg) {}
    
    /**
     * Safely copy packet data using signed length
     * 
     * @param dest Destination buffer
     * @param src Source buffer
     * @param signed_length Packet length as signed integer
     * @param dest_capacity Optional destination buffer capacity
     * @return PacketCopyResult with operation details
     */
    PacketCopyResult copyPacket(void* dest, const void* src, 
                                int signed_length, 
                                size_t dest_capacity = 0) const {
        PacketCopyResult result;
        result.dest_ptr = dest;
        result.src_ptr = src;
        
        // Step 1: Validate signed length
        if (signed_length < 0) {
            result.error_message = "Negative packet length: " + std::to_string(signed_length);
            return result;
        }
        
        // Step 2: Handle zero length
        if (signed_length == 0) {
            if (!config.allow_zero_length) {
                result.error_message = "Zero-length packet not allowed";
                return result;
            }
            result.success = true;
            result.bytes_copied = 0;
            return result;
        }
        
        // Step 3: SAFE CONVERSION - signed int to size_t
        // First cast to unsigned int to preserve bit pattern
        unsigned int unsigned_length = static_cast<unsigned int>(signed_length);
        
        // Additional check: Ensure conversion didn't change value
        if (static_cast<int>(unsigned_length) != signed_length) {
            result.error_message = "Integer conversion anomaly detected";
            return result;
        }
        
        // Step 4: Convert to size_t (may be larger than unsigned int)
        size_t safe_length = static_cast<size_t>(unsigned_length);
        
        // Log the conversion
        logOperation("Length conversion", signed_length, safe_length);
        
        // Step 5: Apply size limits
        if (safe_length > config.max_packet_size) {
            result.error_message = "Packet length " + std::to_string(safe_length) + 
                                  " exceeds maximum " + std::to_string(config.max_packet_size);
            return result;
        }
        
        if (safe_length < config.min_packet_size) {
            result.error_message = "Packet length " + std::to_string(safe_length) + 
                                  " below minimum " + std::to_string(config.min_packet_size);
            return result;
        }
        
        // Step 6: Check destination capacity if provided
        if (config.check_dest_capacity && dest_capacity > 0) {
            if (safe_length > dest_capacity) {
                result.error_message = "Packet length " + std::to_string(safe_length) + 
                                      " exceeds destination capacity " + 
                                      std::to_string(dest_capacity);
                return result;
            }
        }
        
        // Step 7: Validate pointers
        if (!validatePointers(dest, src, safe_length)) {
            result.error_message = "Invalid pointers or overlapping memory regions";
            return result;
        }
        
        // Step 8: Sanity check
        if (!sanityCheck(signed_length, safe_length)) {
            result.error_message = "Packet length failed sanity check";
            return result;
        }
        
        // Step 9: Perform the copy
        try {
            // Use memmove if overlapping, memcpy otherwise
            if (dest == src) {
                // Nothing to copy
            } else {
                // Check for overlap again to decide between memcpy and memmove
                const char* dest_char = static_cast<char*>(dest);
                const char* src_char = static_cast<const char*>(src);
                
                if ((dest_char <= src_char && dest_char + safe_length > src_char) ||
                    (src_char <= dest_char && src_char + safe_length > dest_char)) {
                    // Overlapping - use memmove
                    std::memmove(dest, src, safe_length);
                    if (config.log_operations) {
                        std::cout << "[PacketCopier] Used memmove for overlapping regions\n";
                    }
                } else {
                    // Non-overlapping - use memcpy (potentially faster)
                    std::memcpy(dest, src, safe_length);
                }
            }
            
            result.success = true;
            result.bytes_copied = safe_length;
            result.error_message = "Success";
            
        } catch (const std::exception& e) {
            result.error_message = std::string("memcpy failed: ") + e.what();
        }
        
        return result;
    }
    
    /**
     * Version that throws exceptions on error
     */
    size_t copyPacketOrThrow(void* dest, const void* src, 
                             int signed_length, 
                             size_t dest_capacity = 0) const {
        auto result = copyPacket(dest, src, signed_length, dest_capacity);
        
        if (!result.success) {
            throw PacketError(result.error_message);
        }
        
        return result.bytes_copied;
    }
    
    /**
     * Update configuration
     */
    void setConfig(const PacketCopierConfig& new_config) {
        config = new_config;
    }
};

/**
 * Helper function for simple cases with default configuration
 */
PacketCopyResult safePacketCopy(void* dest, const void* src, 
                                 int signed_length, 
                                 size_t dest_capacity = 0) {
    static const SafePacketCopier default_copier;
    return default_copier.copyPacket(dest, src, signed_length, dest_capacity);
}

/**
 * Demonstration and test function
 */
void demonstratePacketCopier() {
    std::cout << "Safe Packet Copy Demonstration\n";
    std::cout << "==============================\n\n";
    
    // Test data
    const char* test_data = "Hello, this is a test packet with some data!";
    size_t data_len = std::strlen(test_data) + 1;  // Include null terminator
    
    char buffer[1024] = {0};
    
    // Test cases
    struct TestCase {
        int signed_length;
        size_t dest_capacity;
        const char* description;
    };
    
    TestCase tests[] = {
        {static_cast<int>(data_len), sizeof(buffer), "Normal packet"},
        {0, sizeof(buffer), "Zero length packet"},
        {-5, sizeof(buffer), "Negative length"},
        {2000, sizeof(buffer), "Length exceeding buffer"},
        {50, 30, "Length exceeding destination capacity"},
        {std::numeric_limits<int>::max(), sizeof(buffer), "Maximum int value"},
        {-2147483648, sizeof(buffer), "Minimum int value"},
        {10, sizeof(buffer), "Small packet"}
    };
    
    for (const auto& test : tests) {
        std::cout << "\nTest: " << test.description << "\n";
        std::cout << "  Signed length: " << test.signed_length << "\n";
        
        auto result = safePacketCopy(buffer, test_data, 
                                     test.signed_length, 
                                     test.dest_capacity);
        
        std::cout << "  Success: " << (result.success ? "✓" : "✗") << "\n";
        
        if (result.success) {
            std::cout << "  Bytes copied: " << result.bytes_copied << "\n";
            std::cout << "  Data: \"" << buffer << "\"\n";
        } else {
            std::cout << "  Error: " << result.error_message << "\n";
        }
    }
    
    // Demonstrate different security levels
    std::cout << "\n\nSecurity Level Demonstration\n";
    std::cout << "----------------------------\n";
    
    PacketCopierConfig config;
    config.max_packet_size = 100;
    
    // Test suspicious values
    int suspicious_lengths[] = {50, 0xFFFFFFFF, 0xDEADBEEF, 1024, 64};
    
    for (auto level : {SecurityLevel::RELAXED, SecurityLevel::STRICT, SecurityLevel::PARANOID}) {
        config.security_level = level;
        SafePacketCopier copier(config);
        
        std::cout << "\nSecurity Level: ";
        switch(level) {
            case SecurityLevel::RELAXED: std::cout << "RELAXED"; break;
            case SecurityLevel::STRICT: std::cout << "STRICT"; break;
            case SecurityLevel::PARANOID: std::cout << "PARANOID"; break;
        }
        std::cout << "\n";
        
        for (int len : suspicious_lengths) {
            auto result = copier.copyPacket(buffer, test_data, len, sizeof(buffer));
            std::cout << "  Length " << std::setw(10) << len << ": "
                     << (result.success ? "✓ ACCEPTED" : "✗ REJECTED")
                     << (result.success ? "" : " (" + result.error_message + ")")
                     << "\n";
        }
    }
}

int main() {
    try {
        demonstratePacketCopier();
        
        // Example of exception-throwing version
        std::cout << "\n\nException Handling Example\n";
        std::cout << "--------------------------\n";
        
        char dest[50];
        const char* src = "Important packet data";
        
        SafePacketCopier copier;
        
        try {
            // This will throw due to negative length
            size_t copied = copier.copyPacketOrThrow(dest, src, -10, sizeof(dest));
            std::cout << "Copied " << copied << " bytes\n";
        } catch (const PacketError& e) {
            std::cout << "Caught expected error: " << e.what() << "\n";
        }
        
        try {
            // This will succeed
            size_t copied = copier.copyPacketOrThrow(dest, src, 20, sizeof(dest));
            std::cout << "Successfully copied " << copied << " bytes: \"" 
                      << dest << "\"\n";
        } catch (const PacketError& e) {
            std::cout << "Unexpected error: " << e.what() << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}