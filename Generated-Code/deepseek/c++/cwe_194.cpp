#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <vector>
#include <iomanip>

/**
 * Custom exception for configuration errors
 */
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) 
        : std::runtime_error("Configuration Error: " + message) {}
};

/**
 * Result structure for configuration parsing
 */
struct ConfigParseResult {
    bool success;
    std::string key;
    std::string value;
    size_t line_number;
    std::string error_message;
    
    ConfigParseResult() : success(false), line_number(0) {}
};

/**
 * Configuration file parser with security considerations
 */
class ConfigParser {
private:
    std::string filename;
    std::vector<std::pair<std::string, std::string>> config_pairs;
    std::vector<ConfigParseResult> parse_log;
    
    /**
     * Trim whitespace from string
     */
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
    
    /**
     * Validate key name (alphanumeric and underscore only)
     */
    bool isValidKey(const std::string& key) {
        if (key.empty()) return false;
        return std::all_of(key.begin(), key.end(), [](char c) {
            return std::isalnum(c) || c == '_';
        });
    }
    
    /**
     * Check for potential security issues in value
     */
    bool isSecureValue(const std::string& value) {
        // Block potential injection attempts
        std::vector<std::string> dangerous_patterns = {
            ";", "&&", "||", "`", "$(", "${", ">", "<", "|",
            "import", "include", "exec", "system", "eval"
        };
        
        std::string lower_value = value;
        std::transform(lower_value.begin(), lower_value.end(), 
                      lower_value.begin(), ::tolower);
        
        for (const auto& pattern : dangerous_patterns) {
            if (lower_value.find(pattern) != std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
    
public:
    explicit ConfigParser(const std::string& file) : filename(file) {}
    
    /**
     * Parse configuration file
     */
    bool parse() {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw ConfigError("Cannot open configuration file: " + filename);
        }
        
        std::string line;
        size_t line_num = 0;
        bool success = true;
        
        while (std::getline(file, line)) {
            line_num++;
            ConfigParseResult result;
            result.line_number = line_num;
            
            // Remove comments
            size_t comment_pos = line.find('#');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }
            
            line = trim(line);
            
            // Skip empty lines
            if (line.empty()) {
                continue;
            }
            
            // Parse key=value
            size_t equals_pos = line.find('=');
            if (equals_pos == std::string::npos) {
                result.error_message = "Missing '=' separator";
                parse_log.push_back(result);
                success = false;
                continue;
            }
            
            std::string key = trim(line.substr(0, equals_pos));
            std::string value = trim(line.substr(equals_pos + 1));
            
            // Validate key
            if (!isValidKey(key)) {
                result.error_message = "Invalid key name (use alphanumeric and underscore only)";
                parse_log.push_back(result);
                success = false;
                continue;
            }
            
            // Security check
            if (!isSecureValue(value)) {
                result.error_message = "Potentially dangerous value detected";
                parse_log.push_back(result);
                success = false;
                continue;
            }
            
            // Success
            result.success = true;
            result.key = key;
            result.value = value;
            parse_log.push_back(result);
            config_pairs.emplace_back(key, value);
        }
        
        file.close();
        return success;
    }
    
    /**
     * Get value as signed short with validation
     */
    short getAsShort(const std::string& key, short default_value = 0) {
        auto it = std::find_if(config_pairs.begin(), config_pairs.end(),
            [&key](const auto& pair) { return pair.first == key; });
        
        if (it == config_pairs.end()) {
            return default_value;
        }
        
        try {
            // Parse with overflow checking
            long value;
            size_t pos;
            
            try {
                value = std::stol(it->second, &pos);
            } catch (const std::invalid_argument&) {
                throw ConfigError("Key '" + key + "' contains non-numeric value: " + it->second);
            } catch (const std::out_of_range&) {
                throw ConfigError("Key '" + key + "' value out of range for long integer");
            }
            
            // Check for trailing characters
            if (pos != it->second.length()) {
                throw ConfigError("Key '" + key + "' contains trailing non-numeric characters");
            }
            
            // Check range for signed short
            if (value < std::numeric_limits<short>::min() || 
                value > std::numeric_limits<short>::max()) {
                throw ConfigError("Key '" + key + "' value " + it->second + 
                                 " out of range for signed short");
            }
            
            return static_cast<short>(value);
            
        } catch (const std::exception& e) {
            throw ConfigError(std::string("Failed to parse key '") + key + "': " + e.what());
        }
    }
    
    /**
     * Print parse log
     */
    void printParseLog() const {
        std::cout << "\nConfiguration Parse Log:\n";
        std::cout << "========================\n";
        
        for (const auto& result : parse_log) {
            std::cout << "Line " << std::setw(3) << result.line_number << ": ";
            
            if (result.success) {
                std::cout << "✓ " << result.key << " = " << result.value << "\n";
            } else {
                std::cout << "✗ " << result.error_message << "\n";
            }
        }
    }
};

/**
 * Buffer manager class demonstrating safe buffer allocation
 */
class BufferManager {
private:
    size_t buffer_size;
    std::unique_ptr<char[]> buffer;
    bool is_allocated;
    
public:
    explicit BufferManager(size_t size) 
        : buffer_size(size)
        , is_allocated(false) {
        
        std::cout << "BufferManager: Requested size = " << size << " bytes\n";
        
        // Validate size before allocation
        if (size == 0) {
            throw std::invalid_argument("Buffer size cannot be zero");
        }
        
        if (size > 1024 * 1024 * 100) { // 100 MB limit
            throw std::length_error("Buffer size exceeds maximum allowed (100 MB)");
        }
        
        // Attempt allocation
        try {
            buffer = std::make_unique<char[]>(size);
            is_allocated = true;
            
            // Initialize buffer with zeros for security
            std::fill_n(buffer.get(), size, 0);
            
            std::cout << "BufferManager: Successfully allocated " << size 
                     << " bytes at address " << static_cast<void*>(buffer.get()) << "\n";
            
        } catch (const std::bad_alloc& e) {
            throw std::runtime_error("Failed to allocate buffer of size " + 
                                     std::to_string(size) + ": " + e.what());
        }
    }
    
    ~BufferManager() {
        if (is_allocated && buffer) {
            // Securely clear buffer before deallocation
            std::fill_n(buffer.get(), buffer_size, 0);
            std::cout << "BufferManager: Buffer cleared and deallocated\n";
        }
    }
    
    // Disable copying
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator=(const BufferManager&) = delete;
    
    // Enable moving
    BufferManager(BufferManager&& other) noexcept
        : buffer_size(other.buffer_size)
        , buffer(std::move(other.buffer))
        , is_allocated(other.is_allocated) {
        other.buffer_size = 0;
        other.is_allocated = false;
    }
    
    // Accessors
    size_t getSize() const { return buffer_size; }
    char* getBuffer() { return buffer.get(); }
    const char* getBuffer() const { return buffer.get(); }
    bool isValid() const { return is_allocated && buffer != nullptr; }
    
    /**
     * Write test pattern to buffer
     */
    void writeTestPattern() {
        if (!isValid()) return;
        
        for (size_t i = 0; i < buffer_size; ++i) {
            buffer[i] = static_cast<char>('A' + (i % 26));
        }
        std::cout << "BufferManager: Wrote test pattern to buffer\n";
    }
    
    /**
     * Print first few bytes of buffer
     */
    void printBufferPreview(size_t bytes = 64) const {
        if (!isValid()) return;
        
        size_t preview = std::min(bytes, buffer_size);
        std::cout << "Buffer preview (first " << preview << " bytes):\n";
        
        for (size_t i = 0; i < preview; ++i) {
            if (i > 0 && i % 16 == 0) std::cout << "\n";
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                     << static_cast<int>(static_cast<unsigned char>(buffer[i])) << " ";
        }
        std::cout << std::dec << "\n";
    }
};

/**
 * Main program demonstrating safe configuration reading and buffer allocation
 */
int main() {
    std::cout << "Secure Buffer Allocation from Configuration\n";
    std::cout << "============================================\n\n";
    
    // Create a sample configuration file
    std::ofstream config_file("buffer_config.conf");
    config_file << "# Buffer Configuration File\n";
    config_file << "# Size is specified as signed short\n\n";
    config_file << "BUFFER_SIZE = 4096      # 4KB buffer\n";
    config_file << "MAX_CONNECTIONS = 100    # Another setting\n";
    config_file << "ENABLE_LOGGING = true    # Boolean setting\n\n";
    config_file << "BUFFER_SIZE_SMALL = 1024 # 1KB buffer\n";
    config_file << "BUFFER_SIZE_LARGE = 32767 # Max positive short\n";
    config_file << "BUFFER_SIZE_NEG = -512    # Invalid: negative size\n";
    config_file << "BUFFER_SIZE_BIG = 100000  # Invalid: > short max\n";
    config_file << "BUFFER_SIZE_TEXT = five   # Invalid: non-numeric\n";
    config_file.close();
    
    std::cout << "Created sample configuration file: buffer_config.conf\n\n";
    
    try {
        // Parse configuration
        ConfigParser parser("buffer_config.conf");
        
        std::cout << "Parsing configuration...\n";
        if (!parser.parse()) {
            std::cout << "Warning: Some configuration lines had errors\n";
        }
        
        parser.printParseLog();
        std::cout << "\n";
        
        // Define buffer size keys to try
        std::vector<std::string> buffer_keys = {
            "BUFFER_SIZE",
            "BUFFER_SIZE_SMALL",
            "BUFFER_SIZE_LARGE",
            "BUFFER_SIZE_NEG",
            "BUFFER_SIZE_BIG",
            "BUFFER_SIZE_TEXT",
            "NONEXISTENT_KEY"
        };
        
        // Try each buffer size configuration
        for (const auto& key : buffer_keys) {
            std::cout << "\n" << std::string(50, '-') << "\n";
            std::cout << "Processing key: " << key << "\n";
            
            try {
                // Read signed short from config
                short config_short = parser.getAsShort(key, -1); // Default -1 indicates not found
                
                if (config_short == -1 && key != "NONEXISTENT_KEY") {
                    std::cout << "Key '" << key << "' not found in configuration\n";
                    continue;
                }
                
                std::cout << "Read from config: " << config_short << " (signed short)\n";
                
                // Validate the signed short value before conversion
                if (config_short <= 0) {
                    std::cout << "❌ Invalid buffer size: " << config_short 
                             << " (must be positive)\n";
                    continue;
                }
                
                // SAFE CONVERSION: signed short to size_t
                // First cast to unsigned to handle potential sign issues
                unsigned short unsigned_val = static_cast<unsigned short>(config_short);
                size_t buffer_size = static_cast<size_t>(unsigned_val);
                
                std::cout << "Converted to size_t: " << buffer_size << "\n";
                
                // Additional safety check for size_t conversion
                if (buffer_size > std::numeric_limits<size_t>::max() / 2) {
                    std::cout << "⚠️  Warning: Buffer size is very large\n";
                }
                
                // Create buffer with validated size
                BufferManager buffer(buffer_size);
                
                // Demonstrate buffer usage
                buffer.writeTestPattern();
                buffer.printBufferPreview(32);
                
                std::cout << "✓ Buffer allocated successfully\n";
                
            } catch (const ConfigError& e) {
                std::cout << "❌ Configuration error: " << e.what() << "\n";
            } catch (const std::invalid_argument& e) {
                std::cout << "❌ Invalid argument: " << e.what() << "\n";
            } catch (const std::length_error& e) {
                std::cout << "❌ Length error: " << e.what() << "\n";
            } catch (const std::bad_alloc& e) {
                std::cout << "❌ Memory allocation failed: " << e.what() << "\n";
            } catch (const std::exception& e) {
                std::cout << "❌ Unexpected error: " << e.what() << "\n";
            }
        }
        
        // Demonstrate safe conversion principles
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "Safe Conversion Principles:\n";
        std::cout << std::string(50, '=') << "\n";
        
        // Example 1: Direct conversion pitfalls
        short negative_value = -1024;
        std::cout << "Original signed short: " << negative_value << "\n";
        
        // UNSAFE: Direct conversion
        size_t unsafe_size = static_cast<size_t>(negative_value);
        std::cout << "UNSAFE direct conversion to size_t: " << unsafe_size 
                 << " (huge value due to sign extension!)\n";
        
        // SAFE: Two-step conversion
        unsigned short safe_unsigned = static_cast<unsigned short>(negative_value);
        size_t safe_size = static_cast<size_t>(safe_unsigned);
        std::cout << "SAFE two-step conversion: first to unsigned short: " 
                 << safe_unsigned << ", then to size_t: " << safe_size << "\n";
        
        // Example 2: Boundary values
        std::cout << "\nBoundary Value Analysis:\n";
        short test_values[] = {
            -32768,  // Minimum negative
            -1,      // Negative
            0,       // Zero
            1,       // Small positive
            32767,   // Maximum positive
        };
        
        for (short val : test_values) {
            unsigned short us = static_cast<unsigned short>(val);
            size_t st = static_cast<size_t>(us);
            std::cout << "short: " << std::setw(6) << val 
                     << " → unsigned short: " << std::setw(6) << us 
                     << " → size_t: " << st << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}