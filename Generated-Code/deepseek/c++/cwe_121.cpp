#include <iostream>
#include <iomanip>
#include <cstring>
#include <limits>
#include <algorithm>
#include <cctype>
#include <sstream>

/**
 * Custom exception for stack buffer operations
 */
class StackBufferError : public std::runtime_error {
public:
    explicit StackBufferError(const std::string& message) 
        : std::runtime_error("Stack Buffer Error: " + message) {}
};

/**
 * Configuration for stack buffer reading
 */
struct BufferConfig {
    bool clear_buffer_before_read;
    bool validate_input;
    bool allow_empty;
    bool trim_whitespace;
    size_t max_attempts;
    
    BufferConfig() 
        : clear_buffer_before_read(true)
        , validate_input(true)
        , allow_empty(false)
        , trim_whitespace(true)
        , max_attempts(3) {}
};

/**
 * Result structure for buffer read operations
 */
template<size_t SIZE>
struct BufferReadResult {
    char buffer[SIZE];
    bool success;
    size_t bytes_read;
    bool truncated;
    bool validation_failed;
    std::string error_message;
    
    BufferReadResult() : success(false), bytes_read(0), 
                        truncated(false), validation_failed(false) {
        std::fill_n(buffer, SIZE, 0);
    }
    
    void clear() {
        std::fill_n(buffer, SIZE, 0);
        success = false;
        bytes_read = 0;
        truncated = false;
        validation_failed = false;
        error_message.clear();
    }
};

/**
 * Stack-based buffer reader class
 */
template<size_t BUFFER_SIZE>
class StackBufferReader {
private:
    char buffer[BUFFER_SIZE];
    BufferConfig config;
    
    /**
     * Securely clear the buffer
     */
    void secureClear() {
        volatile char* ptr = buffer;
        for (size_t i = 0; i < BUFFER_SIZE; ++i) {
            ptr[i] = 0;
        }
    }
    
    /**
     * Validate input content (basic example)
     */
    bool validateContent(const char* data, size_t length) {
        if (!config.validate_input) return true;
        
        // Example: Check for non-printable characters (except newline/tab)
        for (size_t i = 0; i < length; ++i) {
            unsigned char c = static_cast<unsigned char>(data[i]);
            if (!std::isprint(c) && c != '\0' && c != '\n' && c != '\t' && c != '\r') {
                return false;
            }
        }
        return true;
    }
    
    /**
     * Trim whitespace from the buffer
     */
    void trimBuffer() {
        if (!config.trim_whitespace) return;
        
        // Find first non-whitespace
        size_t start = 0;
        while (start < BUFFER_SIZE - 1 && std::isspace(static_cast<unsigned char>(buffer[start]))) {
            ++start;
        }
        
        // Find last non-whitespace
        size_t end = BUFFER_SIZE - 1;
        while (end > start && std::isspace(static_cast<unsigned char>(buffer[end - 1]))) {
            --end;
        }
        
        // Shift if necessary
        if (start > 0) {
            size_t len = end - start;
            std::memmove(buffer, buffer + start, len);
            buffer[len] = '\0';
        } else if (end < BUFFER_SIZE - 1) {
            buffer[end] = '\0';
        }
    }
    
public:
    StackBufferReader(const BufferConfig& cfg = BufferConfig{}) : config(cfg) {
        secureClear();
    }
    
    /**
     * Method 1: Read using cin.getline()
     */
    BufferReadResult<BUFFER_SIZE> readWithGetline(const std::string& prompt = "Enter data: ") {
        BufferReadResult<BUFFER_SIZE> result;
        
        if (config.clear_buffer_before_read) {
            secureClear();
        }
        
        std::cout << prompt;
        std::cin.getline(buffer, BUFFER_SIZE);
        
        // Check for read failure
        if (std::cin.fail()) {
            if (!std::cin.eof()) {
                result.error_message = "Input exceeded buffer size";
                result.truncated = true;
                // Clear error state and ignore remaining input
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                result.error_message = "End of file reached";
            }
        } else {
            result.bytes_read = std::strlen(buffer);
            
            // Validate content
            if (!validateContent(buffer, result.bytes_read)) {
                result.validation_failed = true;
                result.error_message = "Input contains invalid characters";
                secureClear();
                return result;
            }
            
            // Trim if configured
            trimBuffer();
            result.bytes_read = std::strlen(buffer);
            
            // Check for empty input
            if (result.bytes_read == 0 && !config.allow_empty) {
                result.error_message = "Empty input not allowed";
                return result;
            }
            
            result.success = true;
        }
        
        std::memcpy(result.buffer, buffer, BUFFER_SIZE);
        return result;
    }
    
    /**
     * Method 2: Read character by character with manual control
     */
    BufferReadResult<BUFFER_SIZE> readCharByChar(const std::string& prompt = "Enter data: ") {
        BufferReadResult<BUFFER_SIZE> result;
        
        if (config.clear_buffer_before_read) {
            secureClear();
        }
        
        std::cout << prompt;
        
        size_t index = 0;
        char ch;
        
        while (index < BUFFER_SIZE - 1) {
            ch = std::cin.get();
            
            if (ch == '\n') {
                break;
            }
            
            if (ch == '\b' && index > 0) {  // Handle backspace
                --index;
                continue;
            }
            
            buffer[index++] = ch;
        }
        
        buffer[index] = '\0';
        
        // Clear remaining input if buffer filled
        if (index == BUFFER_SIZE - 1 && ch != '\n') {
            result.truncated = true;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        
        result.bytes_read = index;
        
        // Validate content
        if (!validateContent(buffer, result.bytes_read)) {
            result.validation_failed = true;
            result.error_message = "Input contains invalid characters";
            secureClear();
            return result;
        }
        
        // Trim if configured
        trimBuffer();
        result.bytes_read = std::strlen(buffer);
        
        // Check for empty input
        if (result.bytes_read == 0 && !config.allow_empty) {
            result.error_message = "Empty input not allowed";
            return result;
        }
        
        result.success = true;
        std::memcpy(result.buffer, buffer, BUFFER_SIZE);
        return result;
    }
    
    /**
     * Method 3: Read using cin.get() with delimiter
     */
    BufferReadResult<BUFFER_SIZE> readWithGet(char delimiter = '\n', 
                                               const std::string& prompt = "Enter data: ") {
        BufferReadResult<BUFFER_SIZE> result;
        
        if (config.clear_buffer_before_read) {
            secureClear();
        }
        
        std::cout << prompt;
        std::cin.get(buffer, BUFFER_SIZE, delimiter);
        
        // Check if delimiter caused early termination
        if (std::cin.peek() == delimiter) {
            std::cin.ignore(); // Consume delimiter
        }
        
        if (std::cin.fail()) {
            if (!std::cin.eof()) {
                result.error_message = "Input exceeded buffer size";
                result.truncated = true;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), delimiter);
            }
        } else {
            result.bytes_read = std::strlen(buffer);
            
            if (!validateContent(buffer, result.bytes_read)) {
                result.validation_failed = true;
                result.error_message = "Input contains invalid characters";
                secureClear();
                return result;
            }
            
            trimBuffer();
            result.bytes_read = std::strlen(buffer);
            
            if (result.bytes_read == 0 && !config.allow_empty) {
                result.error_message = "Empty input not allowed";
                return result;
            }
            
            result.success = true;
        }
        
        std::memcpy(result.buffer, buffer, BUFFER_SIZE);
        return result;
    }
    
    /**
     * Method 4: Read with retry logic
     */
    BufferReadResult<BUFFER_SIZE> readWithRetry(const std::string& prompt = "Enter data: ") {
        BufferReadResult<BUFFER_SIZE> result;
        
        for (size_t attempt = 1; attempt <= config.max_attempts; ++attempt) {
            result = readWithGetline(prompt);
            
            if (result.success) {
                return result;
            }
            
            if (attempt < config.max_attempts) {
                std::cout << "Invalid input. Attempt " << attempt 
                          << "/" << config.max_attempts << ". Try again: ";
            }
        }
        
        result.error_message = "Maximum attempts exceeded";
        return result;
    }
    
    /**
     * Method 5: Read formatted input
     */
    template<typename... Args>
    BufferReadResult<BUFFER_SIZE> readFormatted(const char* format, Args... args) {
        BufferReadResult<BUFFER_SIZE> result;
        
        if (config.clear_buffer_before_read) {
            secureClear();
        }
        
        // This is a simplified example - in practice, use scanf_s or istringstream
        std::string input;
        std::getline(std::cin, input);
        std::istringstream iss(input);
        
        // Example: Parse two strings
        char str1[50], str2[50];
        if (iss >> std::setw(50) >> str1 >> std::setw(50) >> str2) {
            // Safely copy to buffer
            snprintf(buffer, BUFFER_SIZE, "%s %s", str1, str2);
            result.success = true;
            result.bytes_read = std::strlen(buffer);
            std::memcpy(result.buffer, buffer, BUFFER_SIZE);
        } else {
            result.error_message = "Failed to parse formatted input";
        }
        
        return result;
    }
    
    /**
     * Get current buffer content
     */
    const char* getBuffer() const {
        return buffer;
    }
    
    /**
     * Reset the buffer
     */
    void reset() {
        secureClear();
    }
};

/**
 * Utility function to print buffer contents (hex dump)
 */
void printBufferHex(const char* buffer, size_t size, size_t max_display = 64) {
    size_t display = std::min(size, max_display);
    
    std::cout << "Buffer hex dump (" << display << " bytes):\n";
    for (size_t i = 0; i < display; ++i) {
        if (i % 16 == 0) {
            std::cout << "\n  " << std::setw(4) << std::setfill('0') << i << ": ";
        }
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(static_cast<unsigned char>(buffer[i])) << " ";
    }
    std::cout << std::dec << "\n";
    
    // Print ASCII representation
    std::cout << "  ASCII: ";
    for (size_t i = 0; i < display; ++i) {
        unsigned char c = static_cast<unsigned char>(buffer[i]);
        if (std::isprint(c)) {
            std::cout << c;
        } else {
            std::cout << '.';
        }
    }
    std::cout << "\n";
}

/**
 * Demonstrate various stack buffer reading techniques
 */
void demonstrateStackBufferReading() {
    std::cout << "Stack Buffer Reading Demonstration\n";
    std::cout << "==================================\n";
    
    // Example 1: Basic reading with getline
    {
        std::cout << "\n--- Example 1: Basic getline reading ---\n";
        StackBufferReader<50> reader;
        auto result = reader.readWithGetline("Enter some text (max 49 chars): ");
        
        if (result.success) {
            std::cout << "Successfully read: \"" << result.buffer << "\"\n";
            std::cout << "Bytes read: " << result.bytes_read << "\n";
        } else {
            std::cout << "Failed: " << result.error_message << "\n";
        }
    }
    
    // Example 2: Character by character with backspace support
    {
        std::cout << "\n--- Example 2: Character-by-character reading ---\n";
        StackBufferReader<20> reader;
        std::cout << "(Backspace supported, max 19 chars)\n";
        auto result = reader.readCharByChar("Enter text: ");
        
        if (result.success) {
            std::cout << "You entered: \"" << result.buffer << "\"\n";
            printBufferHex(result.buffer, result.bytes_read + 1);
        }
    }
    
    // Example 3: Reading with custom delimiter
    {
        std::cout << "\n--- Example 3: Reading with comma delimiter ---\n";
        StackBufferReader<50> reader;
        auto result = reader.readWithGet(',', "Enter text (stop at comma): ");
        
        if (result.success) {
            std::cout << "Read until comma: \"" << result.buffer << "\"\n";
        }
    }
    
    // Example 4: With retry logic
    {
        std::cout << "\n--- Example 4: Reading with retry logic ---\n";
        BufferConfig cfg;
        cfg.max_attempts = 3;
        cfg.allow_empty = false;
        
        StackBufferReader<30> reader(cfg);
        auto result = reader.readWithRetry("Enter non-empty text (3 attempts max): ");
        
        if (result.success) {
            std::cout << "Successfully read: \"" << result.buffer << "\"\n";
        } else {
            std::cout << "Failed after retries: " << result.error_message << "\n";
        }
    }
    
    // Example 5: With whitespace trimming
    {
        std::cout << "\n--- Example 5: With whitespace trimming ---\n";
        BufferConfig cfg;
        cfg.trim_whitespace = true;
        
        StackBufferReader<50> reader(cfg);
        auto result = reader.readWithGetline("Enter text with spaces: ");
        
        if (result.success) {
            std::cout << "Trimmed result: \"" << result.buffer << "\"\n";
            std::cout << "Original would have spaces preserved but trimmed here\n";
        }
    }
}

/**
 * Demonstrate buffer overflow protection
 */
void demonstrateOverflowProtection() {
    std::cout << "\n\nBuffer Overflow Protection\n";
    std::cout << "==========================\n";
    
    // Small buffer to demonstrate overflow protection
    StackBufferReader<10> reader;
    
    std::cout << "Attempting to read into 10-byte buffer (max 9 chars + null)\n";
    auto result = reader.readWithGetline("Enter a long string: ");
    
    if (result.truncated) {
        std::cout << "⚠️  Input was truncated to fit buffer!\n";
        std::cout << "Stored: \"" << result.buffer << "\"\n";
        std::cout << "Bytes stored: " << result.bytes_read << "\n";
    }
    
    printBufferHex(result.buffer, 10);
}

/**
 * Demonstrate input validation
 */
void demonstrateInputValidation() {
    std::cout << "\n\nInput Validation\n";
    std::cout << "================\n";
    
    BufferConfig cfg;
    cfg.validate_input = true;
    
    StackBufferReader<50> reader(cfg);
    
    std::cout << "Attempting to read with validation (only printable chars allowed)\n";
    auto result = reader.readWithGetline("Enter text (try including non-printable chars): ");
    
    if (result.validation_failed) {
        std::cout << "❌ Validation failed: " << result.error_message << "\n";
    } else if (result.success) {
        std::cout << "✓ Valid input: \"" << result.buffer << "\"\n";
    }
}

/**
 * Demonstrate multiple reads from same buffer
 */
void demonstrateMultipleReads() {
    std::cout << "\n\nMultiple Reads from Same Buffer\n";
    std::cout << "===============================\n";
    
    StackBufferReader<50> reader;
    
    for (int i = 1; i <= 3; ++i) {
        std::cout << "\nRead #" << i << ":\n";
        auto result = reader.readWithGetline("Enter value: ");
        
        if (result.success) {
            std::cout << "Stored: \"" << result.buffer << "\"\n";
        }
        
        // Buffer is automatically cleared between reads if configured
    }
}

/**
 * Main demonstration
 */
int main() {
    try {
        std::cout << "========================================\n";
        std::cout << "Stack Buffer Reading Security Examples\n";
        std::cout << "========================================\n";
        
        demonstrateStackBufferReading();
        demonstrateOverflowProtection();
        demonstrateInputValidation();
        demonstrateMultipleReads();
        
        // Additional safety demonstration
        std::cout << "\n\nSecurity Best Practices\n";
        std::cout << "=======================\n";
        
        std::cout << "✓ Always specify buffer size\n";
        std::cout << "✓ Clear buffers before use\n";
        std::cout << "✓ Validate input content\n";
        std::cout << "✓ Handle truncation gracefully\n";
        std::cout << "✓ Use retry logic for critical input\n";
        std::cout << "✓ Clear sensitive data after use\n";
        std::cout << "✓ Never use gets() or unsafe functions\n";
        
        // Example of secure clearing
        {
            std::cout << "\nSecure buffer clearing example:\n";
            char sensitive[50];
            std::cout << "Enter sensitive data (will be cleared): ";
            std::cin.getline(sensitive, 50);
            std::cout << "Data entered: \"" << sensitive << "\"\n";
            
            // Secure clear using volatile
            volatile char* vptr = sensitive;
            for (size_t i = 0; i < 50; ++i) {
                vptr[i] = 0;
            }
            std::cout << "Buffer securely cleared\n";
        }
        
    } catch (const StackBufferError& e) {
        std::cerr << "Stack buffer error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}