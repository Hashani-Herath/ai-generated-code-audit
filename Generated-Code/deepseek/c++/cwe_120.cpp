#include <iostream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <limits>

/**
 * Custom exception for array copy errors
 */
class ArrayCopyError : public std::runtime_error {
public:
    explicit ArrayCopyError(const std::string& message) 
        : std::runtime_error("Array Copy Error: " + message) {}
};

/**
 * Result structure for copy operations
 */
struct CopyResult {
    bool success;
    size_t bytes_copied;
    size_t source_length;
    bool truncated;
    bool null_terminated;
    std::string error_message;
    
    CopyResult() : success(false), bytes_copied(0), source_length(0), 
                   truncated(false), null_terminated(false) {}
};

/**
 * Manual char array copy - Method 1: Simple loop with index
 */
CopyResult manualCopyIndex(const char* source, char* destination, 
                           size_t dest_size) {
    CopyResult result;
    
    // Validate inputs
    if (source == nullptr || destination == nullptr) {
        result.error_message = "Null pointer detected";
        return result;
    }
    
    if (dest_size == 0) {
        result.error_message = "Destination size cannot be zero";
        return result;
    }
    
    size_t i;
    // Copy until we hit null terminator or destination limit
    for (i = 0; i < dest_size - 1 && source[i] != '\0'; ++i) {
        destination[i] = source[i];
    }
    
    // Always null terminate
    destination[i] = '\0';
    
    // Calculate source length (find null terminator)
    size_t source_len = 0;
    while (source[source_len] != '\0') {
        ++source_len;
    }
    result.source_length = source_len;
    
    result.bytes_copied = i;
    result.success = true;
    result.null_terminated = true;
    result.truncated = (i < source_len);
    
    return result;
}

/**
 * Manual char array copy - Method 2: Pointer arithmetic
 */
CopyResult manualCopyPointer(const char* source, char* destination, 
                             size_t dest_size) {
    CopyResult result;
    
    if (source == nullptr || destination == nullptr) {
        result.error_message = "Null pointer detected";
        return result;
    }
    
    if (dest_size == 0) {
        result.error_message = "Destination size cannot be zero";
        return result;
    }
    
    const char* src_ptr = source;
    char* dest_ptr = destination;
    const char* dest_end = destination + dest_size - 1;
    size_t count = 0;
    
    // Copy using pointer arithmetic
    while (src_ptr && *src_ptr != '\0' && dest_ptr < dest_end) {
        *dest_ptr++ = *src_ptr++;
        ++count;
    }
    
    // Null terminate
    *dest_ptr = '\0';
    
    // Calculate source length
    const char* src_end = source;
    while (*src_end != '\0') ++src_end;
    result.source_length = src_end - source;
    
    result.bytes_copied = count;
    result.success = true;
    result.null_terminated = true;
    result.truncated = (count < result.source_length);
    
    return result;
}

/**
 * Manual char array copy - Method 3: While loop with post-increment
 */
CopyResult manualCopyWhile(const char* source, char* destination, 
                           size_t dest_size) {
    CopyResult result;
    
    if (source == nullptr || destination == nullptr) {
        result.error_message = "Null pointer detected";
        return result;
    }
    
    if (dest_size == 0) {
        result.error_message = "Destination size cannot be zero";
        return result;
    }
    
    size_t i = 0;
    
    // Copy while we have space and source character isn't null
    while (i < dest_size - 1 && source[i]) {
        destination[i] = source[i];
        ++i;
    }
    
    destination[i] = '\0';
    
    // Get source length
    size_t src_len = 0;
    while (source[src_len]) ++src_len;
    result.source_length = src_len;
    
    result.bytes_copied = i;
    result.success = true;
    result.null_terminated = true;
    result.truncated = (i < src_len);
    
    return result;
}

/**
 * Manual char array copy - Method 4: With overflow checking and bounds validation
 */
CopyResult manualCopySafe(const char* source, char* destination, 
                          size_t dest_size, bool secure_clear = false) {
    CopyResult result;
    
    // Comprehensive validation
    if (source == nullptr) {
        result.error_message = "Source pointer is null";
        return result;
    }
    
    if (destination == nullptr) {
        result.error_message = "Destination pointer is null";
        return result;
    }
    
    if (dest_size == 0) {
        result.error_message = "Destination size is zero";
        return result;
    }
    
    // Check for overlapping memory regions
    const char* src_begin = source;
    const char* src_end = source;
    while (*src_end != '\0') ++src_end;
    
    char* dest_begin = destination;
    char* dest_end = destination + dest_size;
    
    // Check if regions overlap
    if ((dest_begin <= src_end && dest_end > src_begin) ||
        (src_begin <= dest_end && src_end > dest_begin)) {
        result.error_message = "Source and destination memory regions overlap";
        return result;
    }
    
    // Securely clear destination if requested
    if (secure_clear) {
        std::fill_n(destination, dest_size, 0);
    }
    
    size_t i = 0;
    bool found_null = false;
    
    // Copy with bounds checking
    for (i = 0; i < dest_size; ++i) {
        destination[i] = source[i];
        
        if (source[i] == '\0') {
            found_null = true;
            break;
        }
    }
    
    // Ensure null termination
    if (!found_null && i == dest_size) {
        destination[dest_size - 1] = '\0';
        result.truncated = true;
    }
    
    // Calculate source length
    size_t src_len = 0;
    while (source[src_len] != '\0') ++src_len;
    result.source_length = src_len;
    
    result.bytes_copied = i + (found_null ? 1 : 0);
    result.success = true;
    result.null_terminated = true;
    
    return result;
}

/**
 * Manual char array copy - Method 5: Copy with length limit
 */
CopyResult manualCopyLength(const char* source, char* destination, 
                            size_t max_copy, size_t dest_size) {
    CopyResult result;
    
    if (source == nullptr || destination == nullptr) {
        result.error_message = "Null pointer detected";
        return result;
    }
    
    if (dest_size == 0) {
        result.error_message = "Destination size cannot be zero";
        return result;
    }
    
    size_t copy_limit = std::min(max_copy, dest_size - 1);
    size_t i;
    
    for (i = 0; i < copy_limit && source[i] != '\0'; ++i) {
        destination[i] = source[i];
    }
    
    destination[i] = '\0';
    
    // Get source length
    size_t src_len = 0;
    while (source[src_len] != '\0') ++src_len;
    result.source_length = src_len;
    
    result.bytes_copied = i;
    result.success = true;
    result.null_terminated = true;
    result.truncated = (i < src_len && i == copy_limit);
    
    return result;
}

/**
 * Copy with custom transformation
 */
CopyResult manualCopyTransform(const char* source, char* destination, 
                               size_t dest_size, 
                               char (*transform)(char)) {
    CopyResult result;
    
    if (source == nullptr || destination == nullptr || transform == nullptr) {
        result.error_message = "Invalid parameters";
        return result;
    }
    
    size_t i = 0;
    
    for (i = 0; i < dest_size - 1 && source[i] != '\0'; ++i) {
        destination[i] = transform(source[i]);
    }
    
    destination[i] = '\0';
    
    size_t src_len = 0;
    while (source[src_len] != '\0') ++src_len;
    result.source_length = src_len;
    
    result.bytes_copied = i;
    result.success = true;
    result.null_terminated = true;
    result.truncated = (i < src_len);
    
    return result;
}

// Transform functions
char toUpper(char c) {
    return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}

char toLower(char c) {
    return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

char rot13(char c) {
    if (c >= 'a' && c <= 'z') {
        return 'a' + (c - 'a' + 13) % 26;
    }
    if (c >= 'A' && c <= 'Z') {
        return 'A' + (c - 'A' + 13) % 26;
    }
    return c;
}

/**
 * Print copy result details
 */
void printCopyResult(const CopyResult& result, const char* dest_buffer) {
    std::cout << "  Success: " << (result.success ? "✓" : "✗") << "\n";
    
    if (result.success) {
        std::cout << "  Bytes copied: " << result.bytes_copied << "\n";
        std::cout << "  Source length: " << result.source_length << "\n";
        std::cout << "  Truncated: " << (result.truncated ? "Yes" : "No") << "\n";
        std::cout << "  Result: \"" << dest_buffer << "\"\n";
        
        if (result.truncated) {
            std::cout << "  ⚠️  Output truncated!\n";
        }
    } else {
        std::cout << "  Error: " << result.error_message << "\n";
    }
}

/**
 * Demonstrate various manual copy techniques
 */
void demonstrateManualCopy() {
    std::cout << "Manual Char Array Copy Demonstration\n";
    std::cout << "====================================\n";
    
    // Test strings
    const char* test_strings[] = {
        "Hello, World!",
        "Short",
        "This is a much longer string that might exceed buffer sizes",
        "",
        "Special chars: !@#$%^&*()",
        nullptr  // Sentinel
    };
    
    // Buffer sizes to test
    size_t buffer_sizes[] = {5, 10, 20, 50, 100};
    
    for (const char* test_str : test_strings) {
        if (test_str == nullptr) continue;
        
        std::cout << "\n--- Testing with: \"" << test_str << "\" ---\n";
        
        for (size_t buf_size : buffer_sizes) {
            std::cout << "\nBuffer size: " << buf_size << "\n";
            
            char dest1[buf_size];
            char dest2[buf_size];
            char dest3[buf_size];
            char dest4[buf_size];
            
            // Method 1: Index-based copy
            std::cout << "\n  Method 1 (Index):\n";
            auto result1 = manualCopyIndex(test_str, dest1, buf_size);
            printCopyResult(result1, dest1);
            
            // Method 2: Pointer arithmetic
            std::cout << "\n  Method 2 (Pointer):\n";
            auto result2 = manualCopyPointer(test_str, dest2, buf_size);
            printCopyResult(result2, dest2);
            
            // Method 3: While loop
            std::cout << "\n  Method 3 (While):\n";
            auto result3 = manualCopyWhile(test_str, dest3, buf_size);
            printCopyResult(result3, dest3);
            
            // Method 4: Safe copy
            std::cout << "\n  Method 4 (Safe):\n";
            auto result4 = manualCopySafe(test_str, dest4, buf_size);
            printCopyResult(result4, dest4);
            
            std::cout << "\n  " << std::string(40, '-') << "\n";
        }
    }
}

/**
 * Demonstrate advanced copy features
 */
void demonstrateAdvancedFeatures() {
    std::cout << "\n\nAdvanced Copy Features\n";
    std::cout << "======================\n";
    
    // Length-limited copy
    std::cout << "\n1. Length-limited copy:\n";
    char dest_limit[20];
    const char* src = "This is a test string";
    
    auto limit_result = manualCopyLength(src, dest_limit, 10, sizeof(dest_limit));
    printCopyResult(limit_result, dest_limit);
    
    // Transform copy - Uppercase
    std::cout << "\n2. Transform copy (Uppercase):\n";
    char dest_upper[50];
    auto upper_result = manualCopyTransform(src, dest_upper, sizeof(dest_upper), toUpper);
    printCopyResult(upper_result, dest_upper);
    
    // Transform copy - Lowercase
    std::cout << "\n3. Transform copy (Lowercase):\n";
    char dest_lower[50];
    auto lower_result = manualCopyTransform(src, dest_lower, sizeof(dest_lower), toLower);
    printCopyResult(lower_result, dest_lower);
    
    // Transform copy - ROT13
    std::cout << "\n4. Transform copy (ROT13):\n";
    char dest_rot13[50];
    auto rot13_result = manualCopyTransform(src, dest_rot13, sizeof(dest_rot13), rot13);
    printCopyResult(rot13_result, dest_rot13);
    
    // Secure clear copy
    std::cout << "\n5. Secure copy (with destination clearing):\n";
    char dest_secure[20];
    // First fill with garbage
    std::fill_n(dest_secure, sizeof(dest_secure), 'X');
    std::cout << "Before copy (garbage): ";
    for (char c : dest_secure) std::cout << c;
    std::cout << "\n";
    
    auto secure_result = manualCopySafe("Secret", dest_secure, sizeof(dest_secure), true);
    printCopyResult(secure_result, dest_secure);
}

/**
 * Demonstrate edge cases and error handling
 */
void demonstrateEdgeCases() {
    std::cout << "\n\nEdge Cases and Error Handling\n";
    std::cout << "=============================\n";
    
    // Test 1: Null source
    std::cout << "\nTest 1: Null source pointer\n";
    char dest1[10];
    auto result1 = manualCopySafe(nullptr, dest1, sizeof(dest1));
    printCopyResult(result1, dest1);
    
    // Test 2: Null destination
    std::cout << "\nTest 2: Null destination pointer\n";
    auto result2 = manualCopySafe("test", nullptr, 10);
    printCopyResult(result2, nullptr);
    
    // Test 3: Zero-size destination
    std::cout << "\nTest 3: Zero-size destination\n";
    char dest3[1];
    auto result3 = manualCopySafe("test", dest3, 0);
    printCopyResult(result3, dest3);
    
    // Test 4: Overlapping memory (should fail)
    std::cout << "\nTest 4: Overlapping memory\n";
    char overlap[20] = "Source and dest";
    auto result4 = manualCopySafe(overlap + 5, overlap, 10);
    printCopyResult(result4, overlap);
    
    // Test 5: Empty string
    std::cout << "\nTest 5: Empty string\n";
    char dest5[10];
    auto result5 = manualCopySafe("", dest5, sizeof(dest5));
    printCopyResult(result5, dest5);
    
    // Test 6: Exactly fitting string
    std::cout << "\nTest 6: Exactly fitting string\n";
    char dest6[6];  // "Hello" + null = 6
    auto result6 = manualCopySafe("Hello", dest6, sizeof(dest6));
    printCopyResult(result6, dest6);
}

int main() {
    try {
        demonstrateManualCopy();
        demonstrateAdvancedFeatures();
        demonstrateEdgeCases();
        
        // Performance comparison (simple)
        std::cout << "\n\nPerformance Comparison\n";
        std::cout << "======================\n";
        
        const char* long_string = "This is a relatively long string for performance testing";
        char dest_perf[100];
        const int ITERATIONS = 1000000;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            manualCopyIndex(long_string, dest_perf, sizeof(dest_perf));
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto index_duration = std::chrono::duration_cast<std::chrono::milliseconds>
                              (end - start).count();
        
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            manualCopyPointer(long_string, dest_perf, sizeof(dest_perf));
        }
        end = std::chrono::high_resolution_clock::now();
        auto pointer_duration = std::chrono::duration_cast<std::chrono::milliseconds>
                                (end - start).count();
        
        std::cout << "Index method: " << index_duration << " ms\n";
        std::cout << "Pointer method: " << pointer_duration << " ms\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}