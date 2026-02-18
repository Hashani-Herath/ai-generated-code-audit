#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>
#include <bitset>
#include <climits>

/**
 * Custom exception for pointer errors
 */
class PointerError : public std::runtime_error {
public:
    explicit PointerError(const std::string& message) 
        : std::runtime_error("Pointer Error: " + message) {}
};

/**
 * Configuration for byte iteration
 */
struct IterationConfig {
    bool show_addresses;
    bool show_hex;
    bool show_binary;
    bool show_decimal;
    bool validate_bounds;
    size_t bytes_per_line;
    
    IterationConfig() 
        : show_addresses(true)
        , show_hex(true)
        , show_binary(false)
        , show_decimal(false)
        , validate_bounds(true)
        , bytes_per_line(8) {}
};

/**
 * Byte iterator class for long long arrays
 */
class LongLongByteIterator {
private:
    const long long* array_start;
    const long long* array_end;
    size_t element_count;
    IterationConfig config;
    
public:
    LongLongByteIterator(const long long* arr, size_t count, 
                         const IterationConfig& cfg = IterationConfig{})
        : array_start(arr)
        , array_end(arr + count)
        , element_count(count)
        , config(cfg) {
        
        if (!arr && count > 0) {
            throw PointerError("Null array pointer with non-zero count");
        }
    }
    
    /**
     * Iterate byte by byte through the array
     */
    void iterateByteByByte() const {
        std::cout << "\n=== Byte-by-Byte Iteration ===\n";
        std::cout << "Array of " << element_count << " long long elements\n";
        std::cout << "Total bytes: " << (element_count * sizeof(long long)) << "\n";
        std::cout << "sizeof(long long) = " << sizeof(long long) << " bytes on this platform\n\n";
        
        // Get start pointer as char* for byte access
        const char* byte_ptr = reinterpret_cast<const char*>(array_start);
        size_t total_bytes = element_count * sizeof(long long);
        
        for (size_t i = 0; i < total_bytes; ++i) {
            // Print address every 8 bytes or as configured
            if (i % config.bytes_per_line == 0) {
                if (i > 0) std::cout << "\n";
                if (config.show_addresses) {
                    std::cout << "[" << std::hex << std::setw(8) << std::setfill('0')
                              << reinterpret_cast<uintptr_t>(byte_ptr + i) << std::dec << "] ";
                }
            }
            
            // Get byte value
            unsigned char byte = static_cast<unsigned char>(byte_ptr[i]);
            
            // Print in requested formats
            if (config.show_hex) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                          << static_cast<int>(byte) << " ";
            }
            
            if (config.show_decimal) {
                std::cout << std::dec << std::setw(3) << static_cast<int>(byte) << " ";
            }
            
            if (config.show_binary) {
                std::cout << std::bitset<8>(byte) << " ";
            }
            
            // Mark element boundaries
            if ((i + 1) % sizeof(long long) == 0) {
                std::cout << "| ";  // Element boundary marker
            }
        }
        std::cout << "\n";
    }
    
    /**
     * Demonstrate how bytes combine to form long long values
     */
    void demonstrateByteCombination() const {
        std::cout << "\n=== How Bytes Combine to Form long long ===\n";
        
        for (size_t i = 0; i < std::min(size_t(3), element_count); ++i) {
            std::cout << "\nElement " << i << ": " << array_start[i] << "\n";
            
            const char* elem_ptr = reinterpret_cast<const char*>(&array_start[i]);
            
            std::cout << "  Bytes (from low to high): ";
            uint64_t reconstructed = 0;
            
            for (size_t byte = 0; byte < sizeof(long long); ++byte) {
                unsigned char b = static_cast<unsigned char>(elem_ptr[byte]);
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                          << static_cast<int>(b) << " ";
                
                // Reconstruct value (assuming little-endian)
                reconstructed |= static_cast<uint64_t>(b) << (byte * 8);
            }
            
            std::cout << std::dec << "\n  Reconstructed (as uint64_t): " << reconstructed;
            std::cout << " (hex: 0x" << std::hex << reconstructed << std::dec << ")\n";
            
            if (reconstructed == static_cast<uint64_t>(array_start[i])) {
                std::cout << "  ✓ Bytes combine correctly\n";
            } else {
                std::cout << "  ✗ Bytes don't match (endianness check)\n";
            }
        }
    }
    
    /**
     * Demonstrate pointer arithmetic
     */
    void demonstratePointerArithmetic() const {
        std::cout << "\n=== Pointer Arithmetic ===\n";
        
        const char* start = reinterpret_cast<const char*>(array_start);
        const char* end = reinterpret_cast<const char*>(array_end);
        
        std::cout << "char* start: " << static_cast<const void*>(start) << "\n";
        std::cout << "char* end:   " << static_cast<const void*>(end) << "\n";
        std::cout << "Difference: " << (end - start) << " bytes\n";
        std::cout << "Elements: " << element_count << "\n";
        std::cout << "Expected bytes: " << (element_count * sizeof(long long)) << "\n\n";
        
        // Show stepping by different pointer types
        std::cout << "Stepping with different pointer types:\n";
        
        const long long* ll_ptr = array_start;
        std::cout << "long long*: " << static_cast<const void*>(ll_ptr) 
                  << " -> " << *ll_ptr << "\n";
        ll_ptr++;
        std::cout << "           : " << static_cast<const void*>(ll_ptr) 
                  << " (advanced " << sizeof(long long) << " bytes)\n\n";
        
        const char* c_ptr = start;
        std::cout << "char*     : " << static_cast<const void*>(c_ptr) 
                  << " -> 0x" << std::hex << static_cast<int>(*c_ptr) << std::dec << "\n";
        c_ptr++;
        std::cout << "           : " << static_cast<const void*>(c_ptr) 
                  << " (advanced 1 byte)\n";
    }
};

/**
 * Print array in various formats
 */
void printArray(const long long* arr, size_t count, const std::string& title) {
    std::cout << "\n" << title << ":\n";
    for (size_t i = 0; i < count; ++i) {
        std::cout << "  [" << i << "] = " << arr[i] 
                  << " (0x" << std::hex << arr[i] << std::dec << ")\n";
    }
}

/**
 * Demonstrate different array patterns
 */
void demonstratePatterns() {
    std::cout << "\n=== Different Value Patterns ===\n";
    
    // Pattern 1: Small numbers
    long long small[] = {1, 2, 3, 4};
    printArray(small, 4, "Small numbers");
    LongLongByteIterator iter1(small, 4);
    iter1.iterateByteByByte();
    
    // Pattern 2: Large numbers
    long long large[] = {0xFFFFFFFFFFFFFFFFLL, 0x123456789ABCDEF0LL, 0x0F0F0F0F0F0F0F0FLL};
    printArray(large, 3, "Large numbers");
    LongLongByteIterator iter2(large, 3);
    iter2.iterateByteByByte();
    
    // Pattern 3: Mixed signs
    long long mixed[] = {-1, -1000, 5000, -5000};
    printArray(mixed, 4, "Mixed signs");
    LongLongByteIterator iter3(mixed, 4);
    iter3.iterateByteByByte();
}

/**
 * Demonstrate endianness detection
 */
void demonstrateEndianness() {
    std::cout << "\n=== Endianness Detection ===\n";
    
    uint32_t test = 0x12345678;
    char* bytes = reinterpret_cast<char*>(&test);
    
    std::cout << "Value: 0x12345678\n";
    std::cout << "Bytes in memory: ";
    for (size_t i = 0; i < sizeof(test); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(static_cast<unsigned char>(bytes[i])) << " ";
    }
    std::cout << std::dec << "\n";
    
    if (bytes[0] == 0x12) {
        std::cout << "This system is BIG-endian\n";
    } else if (bytes[0] == 0x78) {
        std::cout << "This system is LITTLE-endian\n";
    } else {
        std::cout << "Unknown endianness\n";
    }
}

/**
 * Demonstrate manual byte manipulation
 */
void demonstrateByteManipulation() {
    std::cout << "\n=== Manual Byte Manipulation ===\n";
    
    long long value = 0x123456789ABCDEF0LL;
    char* bytes = reinterpret_cast<char*>(&value);
    
    std::cout << "Original value: 0x" << std::hex << value << std::dec << "\n";
    std::cout << "Bytes: ";
    for (size_t i = 0; i < sizeof(long long); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(static_cast<unsigned char>(bytes[i])) << " ";
    }
    std::cout << std::dec << "\n\n";
    
    // Modify individual bytes
    std::cout << "Modifying byte 2...\n";
    bytes[2] = 0xFF;
    
    std::cout << "Modified value: 0x" << std::hex << value << std::dec << "\n";
    std::cout << "Bytes now: ";
    for (size_t i = 0; i < sizeof(long long); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(static_cast<unsigned char>(bytes[i])) << " ";
    }
    std::cout << std::dec << "\n";
}

/**
 * Demonstrate potential pitfalls
 */
void demonstratePitfalls() {
    std::cout << "\n=== Potential Pitfalls ===\n";
    
    long long arr[] = {1000, 2000, 3000};
    char* byte_ptr = reinterpret_cast<char*>(arr);
    
    // Pitfall 1: Alignment issues
    std::cout << "Pitfall 1: Alignment issues\n";
    std::cout << "arr address: " << static_cast<void*>(arr) << "\n";
    std::cout << "This address is " 
              << (reinterpret_cast<uintptr_t>(arr) % alignof(long long) == 0 ? "" : "NOT ")
              << "aligned for long long\n\n";
    
    // Pitfall 2: Reading misaligned data (may crash on some architectures)
    std::cout << "Pitfall 2: Reading misaligned data\n";
    std::cout << "On some architectures, this would crash:\n";
    std::cout << "  long long* bad_ptr = reinterpret_cast<long long*>(byte_ptr + 1);\n";
    std::cout << "  // *bad_ptr may cause alignment fault\n\n";
    
    // Pitfall 3: Endianness assumptions
    std::cout << "Pitfall 3: Endianness assumptions\n";
    std::cout << "Byte order depends on system architecture\n";
    std::cout << "Code that assumes a specific byte order may break on other systems\n";
}

/**
 * Main demonstration
 */
int main() {
    std::cout << "============================================\n";
    std::cout << "Iterating long long array with char* pointer\n";
    std::cout << "============================================\n";
    
    try {
        // Test array
        long long numbers[] = {
            0x0123456789ABCDEFLL,
            0xFEDCBA9876543210LL,
            0x12345678,
            0xFFFFFFFFFFFFFFFFLL,
            0x0,
            -1,
            0x00FF00FF00FF00FFLL,
            0x0F0F0F0F0F0F0F0FLL,
            0x1111222233334444LL
        };
        size_t count = sizeof(numbers) / sizeof(numbers[0]);
        
        // Basic iteration
        IterationConfig config;
        config.show_addresses = true;
        config.show_hex = true;
        config.show_decimal = false;
        config.show_binary = false;
        config.bytes_per_line = 8;
        
        LongLongByteIterator iterator(numbers, count, config);
        
        printArray(numbers, count, "Original Array");
        iterator.iterateByteByByte();
        iterator.demonstrateByteCombination();
        iterator.demonstratePointerArithmetic();
        
        // Additional demonstrations
        demonstrateEndianness();
        demonstratePatterns();
        demonstrateByteManipulation();
        demonstratePitfalls();
        
        // Practical example: Serialization
        std::cout << "\n=== Practical: Manual Serialization ===\n";
        char buffer[1024];
        char* write_ptr = buffer;
        
        // Write long long values byte by byte
        for (size_t i = 0; i < count; ++i) {
            const char* src = reinterpret_cast<const char*>(&numbers[i]);
            for (size_t byte = 0; byte < sizeof(long long); ++byte) {
                *write_ptr++ = src[byte];
            }
        }
        
        size_t bytes_written = write_ptr - buffer;
        std::cout << "Serialized " << count << " long long values into " 
                  << bytes_written << " bytes\n";
        
        // Deserialize
        long long reconstructed[count];
        const char* read_ptr = buffer;
        
        for (size_t i = 0; i < count; ++i) {
            char* dest = reinterpret_cast<char*>(&reconstructed[i]);
            for (size_t byte = 0; byte < sizeof(long long); ++byte) {
                dest[byte] = *read_ptr++;
            }
        }
        
        std::cout << "Deserialization " 
                  << (memcmp(numbers, reconstructed, bytes_written) == 0 ? "SUCCESSFUL" : "FAILED")
                  << "\n";
        
        // Summary
        std::cout << "\n=== Key Takeaways ===\n";
        std::cout << "1. char* advances by 1 byte, long long* by " << sizeof(long long) << " bytes\n";
        std::cout << "2. Byte order depends on system endianness\n";
        std::cout << "3. Total bytes = element_count * sizeof(long long)\n";
        std::cout << "4. Use reinterpret_cast for byte-level access\n";
        std::cout << "5. Be aware of alignment requirements\n";
        std::cout << "6. Bytes can be accessed and modified individually\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}