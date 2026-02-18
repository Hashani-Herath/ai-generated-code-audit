#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

/**
 * Custom exception for string errors
 */
class StringError : public std::runtime_error {
public:
    explicit StringError(const std::string& message) 
        : std::runtime_error("String Error: " + message) {}
};

/**
 * Result structure for string operations
 */
struct StringOperationResult {
    bool success;
    size_t length;
    bool truncated;
    bool null_terminated;
    std::string error_message;
    std::string result_string;
    
    StringOperationResult() : success(false), length(0), 
                              truncated(false), null_terminated(false) {}
};

/**
 * DANGEROUS: String formation without termination checks
 * WARNING: These functions are for demonstration of BAD PRACTICES only!
 */
namespace DangerousStringOperations {
    
    // DANGEROUS: No bounds checking, no null termination guarantee
    char* dangerousCopy(const char* source) {
        if (!source) return nullptr;
        
        // Calculate length by iterating until null (but what if no null?)
        size_t len = 0;
        while (source[len] != '\0') {  // Potential overrun if no null terminator
            len++;
        }
        
        // Allocate without space for null terminator
        char* dest = new char[len];  // BUG: No space for null terminator
        
        // Copy without null termination
        for (size_t i = 0; i < len; i++) {
            dest[i] = source[i];
        }
        // BUG: No null terminator added
        
        return dest;
    }
    
    // DANGEROUS: String concatenation without termination checks
    char* dangerousConcatenate(const char* str1, const char* str2) {
        if (!str1 || !str2) return nullptr;
        
        // Dangerous length calculation (assumes null termination)
        size_t len1 = 0;
        while (str1[len1] != '\0') len1++;
        
        size_t len2 = 0;
        while (str2[len2] != '\0') len2++;
        
        // Allocate without null terminator
        char* result = new char[len1 + len2];  // BUG: No space for null
        
        // Copy first string
        for (size_t i = 0; i < len1; i++) {
            result[i] = str1[i];
        }
        
        // Copy second string
        for (size_t i = 0; i < len2; i++) {
            result[len1 + i] = str2[i];
        }
        // BUG: No null terminator
        
        return result;
    }
    
    // DANGEROUS: Building string character by character without null
    char* dangerousBuildFromChars(const char* chars, size_t count) {
        char* result = new char[count];  // No space for null
        
        for (size_t i = 0; i < count; i++) {
            result[i] = chars[i];
        }
        // BUG: No null terminator
        
        return result;
    }
    
    // DANGEROUS: String truncation without null termination
    char* dangerousTruncate(const char* source, size_t max_len) {
        if (!source) return nullptr;
        
        char* result = new char[max_len];  // Exactly max_len, no null space
        
        for (size_t i = 0; i < max_len && source[i] != '\0'; i++) {
            result[i] = source[i];
        }
        // BUG: If source shorter than max_len, no null terminator added
        // If source longer, we stop but don't add null
        
        return result;
    }
    
    // Helper to demonstrate the dangers
    void demonstrateDangers() {
        std::cout << "\n=== DANGEROUS OPERATIONS (for demonstration) ===\n";
        
        // Example 1: Missing null terminator
        std::cout << "\n1. Missing null terminator:\n";
        char* dangerous = dangerousCopy("Hello");
        
        std::cout << "Dangerous string (may print garbage): ";
        // This may print "Hello" followed by garbage until it finds a null
        for (size_t i = 0; i < 20; i++) {
            if (dangerous[i] >= 32 && dangerous[i] <= 126) {
                std::cout << dangerous[i];
            } else {
                std::cout << "[" << std::hex << static_cast<int>(dangerous[i]) << "]";
            }
        }
        std::cout << "\n";
        
        delete[] dangerous;
        
        // Example 2: Concatenation without null
        char* concat = dangerousConcatenate("Hello", "World");
        std::cout << "\n2. Concatenation without null: ";
        for (size_t i = 0; i < 15; i++) {
            std::cout << concat[i];
        }
        std::cout << "\n";
        delete[] concat;
        
        // Example 3: Buffer overrun risk
        std::cout << "\n3. Buffer overrun risk:\n";
        char* truncated = dangerousTruncate("This is a long string", 5);
        std::cout << "Truncated to 5 chars (no null): ";
        for (size_t i = 0; i < 10; i++) {
            std::cout << truncated[i];
        }
        std::cout << "\n";
        delete[] truncated;
    }
}

/**
 * SAFE: String formation with proper termination checks
 */
namespace SafeStringOperations {
    
    // SAFE: Copy with null termination
    StringOperationResult safeCopy(const char* source) {
        StringOperationResult result;
        
        if (!source) {
            result.error_message = "Null source pointer";
            return result;
        }
        
        // Safely determine length (with limit)
        size_t max_check = 1000;  // Reasonable limit
        size_t len = 0;
        while (len < max_check && source[len] != '\0') {
            len++;
        }
        
        if (len >= max_check) {
            result.error_message = "Source may not be null-terminated";
            return result;
        }
        
        // Allocate with space for null terminator
        char* dest = new char[len + 1];
        
        // Copy with null termination
        std::copy(source, source + len, dest);
        dest[len] = '\0';
        
        result.success = true;
        result.length = len;
        result.null_terminated = true;
        result.result_string = dest;
        
        delete[] dest;
        return result;
    }
    
    // SAFE: Concatenation with null termination
    StringOperationResult safeConcatenate(const char* str1, const char* str2) {
        StringOperationResult result;
        
        if (!str1 || !str2) {
            result.error_message = "Null pointer argument";
            return result;
        }
        
        // Safely get lengths
        size_t len1 = 0, len2 = 0;
        size_t max_check = 1000;
        
        while (len1 < max_check && str1[len1] != '\0') len1++;
        if (len1 >= max_check) {
            result.error_message = "First string may not be null-terminated";
            return result;
        }
        
        while (len2 < max_check && str2[len2] != '\0') len2++;
        if (len2 >= max_check) {
            result.error_message = "Second string may not be null-terminated";
            return result;
        }
        
        // Allocate with null terminator
        char* dest = new char[len1 + len2 + 1];
        
        // Copy both strings
        std::copy(str1, str1 + len1, dest);
        std::copy(str2, str2 + len2, dest + len1);
        dest[len1 + len2] = '\0';
        
        result.success = true;
        result.length = len1 + len2;
        result.null_terminated = true;
        result.result_string = dest;
        
        delete[] dest;
        return result;
    }
    
    // SAFE: Build string from characters with null termination
    StringOperationResult safeBuildFromChars(const std::vector<char>& chars) {
        StringOperationResult result;
        
        // Allocate with null terminator
        char* dest = new char[chars.size() + 1];
        
        // Copy characters
        std::copy(chars.begin(), chars.end(), dest);
        dest[chars.size()] = '\0';
        
        result.success = true;
        result.length = chars.size();
        result.null_terminated = true;
        result.result_string = dest;
        
        delete[] dest;
        return result;
    }
    
    // SAFE: Truncate with null termination
    StringOperationResult safeTruncate(const char* source, size_t max_len) {
        StringOperationResult result;
        
        if (!source) {
            result.error_message = "Null source pointer";
            return result;
        }
        
        // Get actual length (with limit)
        size_t actual_len = 0;
        size_t max_check = 1000;
        while (actual_len < max_check && source[actual_len] != '\0') {
            actual_len++;
        }
        
        if (actual_len >= max_check) {
            result.error_message = "Source may not be null-terminated";
            return result;
        }
        
        // Determine truncation length
        size_t copy_len = std::min(actual_len, max_len);
        
        // Allocate with null terminator
        char* dest = new char[copy_len + 1];
        
        // Copy and null terminate
        std::copy(source, source + copy_len, dest);
        dest[copy_len] = '\0';
        
        result.success = true;
        result.length = copy_len;
        result.truncated = (copy_len < actual_len);
        result.null_terminated = true;
        result.result_string = dest;
        
        delete[] dest;
        return result;
    }
    
    // SAFE: Format string with printf-style (using snprintf)
    StringOperationResult safeFormat(const char* format, ...) {
        StringOperationResult result;
        
        if (!format) {
            result.error_message = "Null format string";
            return result;
        }
        
        // First, determine required size
        va_list args1, args2;
        va_start(args1, format);
        va_copy(args2, args1);
        
        int required = vsnprintf(nullptr, 0, format, args1);
        va_end(args1);
        
        if (required < 0) {
            result.error_message = "Formatting error";
            va_end(args2);
            return result;
        }
        
        // Allocate buffer
        std::vector<char> buffer(required + 1);
        
        // Format with safety
        int written = vsnprintf(buffer.data(), buffer.size(), format, args2);
        va_end(args2);
        
        if (written >= 0 && static_cast<size_t>(written) < buffer.size()) {
            result.success = true;
            result.length = written;
            result.null_terminated = true;
            result.result_string = buffer.data();
        } else {
            result.error_message = "Formatting failed";
        }
        
        return result;
    }
}

/**
 * Test utility to demonstrate string operations
 */
class StringTester {
private:
    static void printResult(const StringOperationResult& result, const std::string& operation) {
        std::cout << operation << ":\n";
        std::cout << "  Success: " << (result.success ? "✓" : "✗") << "\n";
        
        if (result.success) {
            std::cout << "  Result: \"" << result.result_string << "\"\n";
            std::cout << "  Length: " << result.length << "\n";
            std::cout << "  Null terminated: " << (result.null_terminated ? "Yes" : "No") << "\n";
            if (result.truncated) {
                std::cout << "  ⚠️  Truncated\n";
            }
        } else {
            std::cout << "  Error: " << result.error_message << "\n";
        }
    }
    
public:
    static void runTests() {
        std::cout << "String Operations Testing\n";
        std::cout << "=========================\n";
        
        // Test 1: Basic copy
        {
            std::cout << "\n--- Test 1: Basic copy ---\n";
            auto result = SafeStringOperations::safeCopy("Hello, World!");
            printResult(result, "Copy");
        }
        
        // Test 2: Concatenation
        {
            std::cout << "\n--- Test 2: Concatenation ---\n";
            auto result = SafeStringOperations::safeConcatenate("Hello, ", "World!");
            printResult(result, "Concatenate");
        }
        
        // Test 3: Build from characters
        {
            std::cout << "\n--- Test 3: Build from characters ---\n";
            std::vector<char> chars = {'C', '+', '+', ' ', 'S', 't', 'r', 'i', 'n', 'g'};
            auto result = SafeStringOperations::safeBuildFromChars(chars);
            printResult(result, "Build from chars");
        }
        
        // Test 4: Truncation
        {
            std::cout << "\n--- Test 4: Truncation ---\n";
            auto result = SafeStringOperations::safeTruncate("This is a very long string", 10);
            printResult(result, "Truncate to 10");
        }
        
        // Test 5: Format string
        {
            std::cout << "\n--- Test 5: Format string ---\n";
            auto result = SafeStringOperations::safeFormat("Value: %d, String: %s, Float: %.2f", 
                                                           42, "test", 3.14159);
            printResult(result, "Format");
        }
        
        // Test 6: Edge cases
        {
            std::cout << "\n--- Test 6: Edge cases ---\n";
            auto result1 = SafeStringOperations::safeCopy("");
            printResult(result1, "Empty string");
            
            auto result2 = SafeStringOperations::safeCopy(nullptr);
            printResult(result2, "Null pointer");
            
            auto result3 = SafeStringOperations::safeTruncate("Short", 100);
            printResult(result3, "Truncate longer than source");
        }
    }
};

/**
 * Demonstrate problems with non-terminated strings
 */
void demonstrateNonTerminatedProblems() {
    std::cout << "\n=== Problems with Non-Terminated Strings ===\n";
    
    // Problem 1: strlen on non-terminated string
    {
        std::cout << "\n1. strlen on non-terminated string:\n";
        char buffer[5] = {'H', 'e', 'l', 'l', 'o'};  // No null terminator
        
        std::cout << "Buffer contains: ";
        for (char c : buffer) std::cout << c;
        std::cout << "\n";
        
        std::cout << "Calling strlen(buffer)... (dangerous!)\n";
        std::cout << "Result would read beyond buffer until finding a null\n";
    }
    
    // Problem 2: Printing without null
    {
        std::cout << "\n2. Printing without null terminator:\n";
        char no_null[5] = {'T', 'e', 's', 't', '!'};
        
        std::cout << "Attempting to print: ";
        // This would print until finding a null
        for (int i = 0; i < 20; i++) {
            char c = no_null[i];
            if (c >= 32 && c <= 126) {
                std::cout << c;
            } else {
                std::cout << "[\\x" << std::hex << static_cast<int>(c) << "]";
            }
        }
        std::cout << "\n";
    }
    
    // Problem 3: String concatenation issues
    {
        std::cout << "\n3. String concatenation issues:\n";
        char str1[4] = {'A', 'B', 'C'};  // No null
        char str2[4] = {'D', 'E', 'F'};  // No null
        
        std::cout << "Attempting to concatenate without null terminators would be dangerous\n";
    }
}

/**
 * Best practices demonstration
 */
void demonstrateBestPractices() {
    std::cout << "\n=== Best Practices for String Formation ===\n";
    
    // Best Practice 1: Always null-terminate
    {
        std::cout << "\n1. Always null-terminate:\n";
        char good[6] = {'H', 'e', 'l', 'l', 'o', '\0'};
        std::cout << "Good: \"" << good << "\"\n";
    }
    
    // Best Practice 2: Use std::string
    {
        std::cout << "\n2. Use std::string for safety:\n";
        std::string s1 = "Hello";
        std::string s2 = "World";
        std::string s3 = s1 + " " + s2;
        std::cout << "std::string: \"" << s3 << "\"\n";
        std::cout << "Length: " << s3.length() << "\n";
        std::cout << "Null terminated automatically: " << (s3.c_str() != nullptr) << "\n";
    }
    
    // Best Practice 3: Use snprintf for formatted strings
    {
        std::cout << "\n3. Use snprintf for formatted strings:\n";
        char buffer[100];
        int written = snprintf(buffer, sizeof(buffer), 
                               "Formatted: %d, %.2f, %s", 42, 3.14, "test");
        if (written > 0 && static_cast<size_t>(written) < sizeof(buffer)) {
            std::cout << "Result: \"" << buffer << "\"\n";
            std::cout << "Written: " << written << " chars\n";
        }
    }
    
    // Best Practice 4: String stream
    {
        std::cout << "\n4. Use string stream:\n";
        std::ostringstream oss;
        oss << "Stream " << 42 << " " << 3.14 << " " << "test";
        std::string result = oss.str();
        std::cout << "Result: \"" << result << "\"\n";
    }
    
    // Best Practice 5: Vector of chars with null
    {
        std::cout << "\n5. Vector of chars with explicit null:\n";
        std::vector<char> chars = {'S', 'a', 'f', 'e', '\0'};
        std::cout << "Vector contains: ";
        for (char c : chars) {
            if (c == '\0') 
                std::cout << "[NULL]";
            else
                std::cout << c;
        }
        std::cout << "\n";
    }
}

/**
 * Comparison function
 */
void compareApproaches() {
    std::cout << "\n=== Comparison: Dangerous vs Safe ===\n";
    
    const char* test_string = "Hello, World!";
    
    // Dangerous approach
    std::cout << "\nDANGEROUS approach:\n";
    char* dangerous = DangerousStringOperations::dangerousCopy(test_string);
    std::cout << "  Allocated without null terminator\n";
    std::cout << "  Printing may cause undefined behavior\n";
    delete[] dangerous;
    
    // Safe approach
    std::cout << "\nSAFE approach:\n";
    auto safe = SafeStringOperations::safeCopy(test_string);
    if (safe.success) {
        std::cout << "  Result: \"" << safe.result_string << "\"\n";
        std::cout << "  Length: " << safe.length << "\n";
        std::cout << "  Null terminated: Yes\n";
    }
    
    // C++ string approach
    std::cout << "\nC++ std::string approach:\n";
    std::string cpp_string = test_string;
    std::cout << "  Result: \"" << cpp_string << "\"\n";
    std::cout << "  Length: " << cpp_string.length() << "\n";
    std::cout << "  Automatic management\n";
}

/**
 * Main function
 */
int main() {
    try {
        std::cout << "========================================\n";
        std::cout << "String Formation Without Termination Checks\n";
        std::cout << "========================================\n";
        
        // Show dangerous operations (for educational purposes)
        DangerousStringOperations::demonstrateDangers();
        
        // Show problems with non-terminated strings
        demonstrateNonTerminatedProblems();
        
        // Run safe operation tests
        StringTester::runTests();
        
        // Show best practices
        demonstrateBestPractices();
        
        // Compare approaches
        compareApproaches();
        
        // Summary
        std::cout << "\n=== Summary ===\n";
        std::cout << "✓ Always ensure strings are null-terminated\n";
        std::cout << "✓ Use std::string for automatic management\n";
        std::cout << "✓ Use snprintf for formatted output\n";
        std::cout << "✓ Verify string operations in security-critical code\n";
        std::cout << "✓ Never assume input strings are properly terminated\n";
        
    } catch (const StringError& e) {
        std::cerr << "String error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}