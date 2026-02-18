#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>

/**
 * Custom exception for array access errors
 */
class ArrayAccessError : public std::runtime_error {
public:
    explicit ArrayAccessError(const std::string& message) 
        : std::runtime_error("Array Access Error: " + message) {}
};

/**
 * Configuration for array access validation
 */
struct AccessConfig {
    size_t max_limit;              // Maximum allowed index (exclusive)
    bool allow_negative_check;      // Check for negative values
    bool use_zero_based;            // Use 0-based indexing
    bool throw_on_error;            // Throw exception vs return error code
    bool log_attempts;               // Log access attempts
    size_t max_attempts;              // Maximum retry attempts
    
    AccessConfig() 
        : max_limit(100)
        , allow_negative_check(true)
        , use_zero_based(true)
        , throw_on_error(false)
        , log_attempts(true)
        , max_attempts(3) {}
};

/**
 * Result structure for array access operations
 */
struct ArrayAccessResult {
    bool success;
    size_t validated_index;
    int original_input;
    std::string error_message;
    bool negative_input;
    bool out_of_bounds;
    size_t attempts_made;
    
    ArrayAccessResult() : success(false), validated_index(0), original_input(0),
                          negative_input(false), out_of_bounds(false), attempts_made(0) {}
};

/**
 * Safe array access validator
 */
class SafeArrayAccess {
private:
    AccessConfig config;
    
    /**
     * Validate integer is within bounds
     */
    bool isWithinBounds(int value) const {
        if (config.allow_negative_check && value < 0) {
            return false;
        }
        
        size_t unsigned_value = static_cast<size_t>(value);
        return unsigned_value < config.max_limit;
    }
    
    /**
     * Convert user input to array index
     */
    size_t convertToIndex(int user_input) const {
        if (!config.use_zero_based && user_input > 0) {
            return static_cast<size_t>(user_input - 1);
        }
        return static_cast<size_t>(user_input);
    }
    
    /**
     * Parse integer from string with validation
     */
    bool parseInteger(const std::string& input, int& result) const {
        if (input.empty()) return false;
        
        // Check for valid integer format
        char* endptr;
        const char* cstr = input.c_str();
        long value = strtol(cstr, &endptr, 10);
        
        // Check if conversion failed or there are trailing characters
        if (*endptr != '\0' || endptr == cstr) {
            return false;
        }
        
        // Check if value fits in int
        if (value > std::numeric_limits<int>::max() || 
            value < std::numeric_limits<int>::min()) {
            return false;
        }
        
        result = static_cast<int>(value);
        return true;
    }
    
    /**
     * Log access attempt
     */
    void logAccess(const ArrayAccessResult& result, int value) const {
        if (!config.log_attempts) return;
        
        std::cout << "[Access Log] ";
        if (result.success) {
            std::cout << "SUCCESS: " << value << " -> index " << result.validated_index << "\n";
        } else {
            std::cout << "FAILURE: " << value << " - " << result.error_message << "\n";
        }
    }
    
public:
    explicit SafeArrayAccess(const AccessConfig& cfg = AccessConfig{}) : config(cfg) {}
    
    /**
     * Method 1: Validate single integer input
     */
    ArrayAccessResult validateInput(int user_input) {
        ArrayAccessResult result;
        result.original_input = user_input;
        
        // Check for negative values
        if (config.allow_negative_check && user_input < 0) {
            result.negative_input = true;
            result.error_message = "Negative index not allowed: " + std::to_string(user_input);
            logAccess(result, user_input);
            return result;
        }
        
        // Convert to index
        size_t index = convertToIndex(user_input);
        
        // Check bounds
        if (index >= config.max_limit) {
            result.out_of_bounds = true;
            result.error_message = "Index " + std::to_string(user_input) + 
                                   " out of bounds (max: " + std::to_string(config.max_limit - 1) + ")";
            logAccess(result, user_input);
            return result;
        }
        
        // Success
        result.success = true;
        result.validated_index = index;
        logAccess(result, user_input);
        
        return result;
    }
    
    /**
     * Method 2: Read and validate from user input
     */
    ArrayAccessResult readAndValidate(const std::string& prompt = "Enter index: ") {
        ArrayAccessResult result;
        
        std::string input;
        int attempts = 0;
        
        while (attempts < config.max_attempts) {
            std::cout << prompt;
            std::getline(std::cin, input);
            
            int user_value;
            if (!parseInteger(input, user_value)) {
                result.error_message = "Invalid number format: \"" + input + "\"";
                attempts++;
                std::cout << "Invalid input. ";
                continue;
            }
            
            result = validateInput(user_value);
            result.attempts_made = attempts + 1;
            
            if (result.success || config.throw_on_error) {
                break;
            }
            
            attempts++;
            if (attempts < config.max_attempts) {
                std::cout << result.error_message << " Try again.\n";
            }
        }
        
        if (!result.success && config.throw_on_error) {
            throw ArrayAccessError(result.error_message);
        }
        
        return result;
    }
    
    /**
     * Method 3: Batch validation
     */
    std::vector<ArrayAccessResult> validateBatch(const std::vector<int>& inputs) {
        std::vector<ArrayAccessResult> results;
        results.reserve(inputs.size());
        
        for (int input : inputs) {
            results.push_back(validateInput(input));
        }
        
        return results;
    }
    
    /**
     * Method 4: Safe array access with element retrieval
     */
    template<typename T>
    ArrayAccessResult accessArray(const std::vector<T>& array, int user_input, T& value) {
        auto result = validateInput(user_input);
        
        if (result.success) {
            value = array[result.validated_index];
        }
        
        return result;
    }
    
    /**
     * Method 5: Safe array access with pointer return
     */
    template<typename T>
    ArrayAccessResult accessArrayPtr(const std::vector<T>& array, int user_input, const T*& ptr) {
        auto result = validateInput(user_input);
        
        if (result.success) {
            ptr = &array[result.validated_index];
        } else {
            ptr = nullptr;
        }
        
        return result;
    }
    
    /**
     * Method 6: Range-based validation
     */
    bool isInRange(int start, int end, int value) const {
        if (start > end) std::swap(start, end);
        
        auto start_result = validateInput(start);
        auto end_result = validateInput(end);
        
        if (!start_result.success || !end_result.success) {
            return false;
        }
        
        auto value_result = validateInput(value);
        if (!value_result.success) return false;
        
        return value_result.validated_index >= start_result.validated_index &&
               value_result.validated_index <= end_result.validated_index;
    }
    
    /**
     * Update configuration
     */
    void setConfig(const AccessConfig& new_config) {
        config = new_config;
    }
};

/**
 * Safe array wrapper with built-in bounds checking
 */
template<typename T, size_t N>
class SafeArray {
private:
    T data[N];
    SafeArrayAccess validator;
    
public:
    SafeArray() {
        // Initialize with default values
        for (size_t i = 0; i < N; ++i) {
            data[i] = T();
        }
    }
    
    SafeArray(std::initializer_list<T> init) {
        size_t i = 0;
        for (const auto& val : init) {
            if (i < N) data[i++] = val;
        }
    }
    
    /**
     * Safe element access
     */
    ArrayAccessResult get(int user_index, T& value) {
        AccessConfig cfg;
        cfg.max_limit = N;
        validator.setConfig(cfg);
        
        return validator.accessArray(std::vector<T>(data, data + N), user_index, value);
    }
    
    /**
     * Safe element modification
     */
    ArrayAccessResult set(int user_index, const T& new_value) {
        AccessConfig cfg;
        cfg.max_limit = N;
        validator.setConfig(cfg);
        
        auto result = validator.validateInput(user_index);
        if (result.success) {
            data[result.validated_index] = new_value;
        }
        
        return result;
    }
    
    /**
     * Display array contents
     */
    void display() const {
        std::cout << "Array[" << N << "]: [";
        for (size_t i = 0; i < N; ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << data[i];
        }
        std::cout << "]\n";
    }
};

/**
 * Demonstrate various validation techniques
 */
void demonstrateValidation() {
    std::cout << "Array Index Validation Demonstration\n";
    std::cout << "====================================\n";
    
    // Test array
    std::vector<int> test_array = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    const size_t MAX_SIZE = test_array.size();
    
    std::cout << "Array size: " << MAX_SIZE << "\n";
    std::cout << "Array contents: ";
    for (size_t i = 0; i < MAX_SIZE; ++i) {
        std::cout << test_array[i] << " ";
    }
    std::cout << "\n\n";
    
    // Create validator
    AccessConfig config;
    config.max_limit = MAX_SIZE;
    SafeArrayAccess validator(config);
    
    // Test cases
    std::vector<int> test_inputs = {5, -1, 10, 0, 3, 100, -5, 7, 9, 20};
    
    std::cout << "--- Basic Validation Tests ---\n";
    for (int input : test_inputs) {
        auto result = validator.validateInput(input);
        
        std::cout << "Input: " << std::setw(3) << input << " -> ";
        if (result.success) {
            std::cout << "VALID (index " << result.validated_index << ")";
            std::cout << " value: " << test_array[result.validated_index];
        } else {
            std::cout << "INVALID: " << result.error_message;
        }
        std::cout << "\n";
    }
    
    // Test with 1-based indexing
    std::cout << "\n--- 1-based indexing ---\n";
    AccessConfig config1based;
    config1based.max_limit = MAX_SIZE;
    config1based.use_zero_based = false;
    SafeArrayAccess validator1based(config1based);
    
    for (int input : {1, 5, 10, 11}) {
        auto result = validator1based.validateInput(input);
        std::cout << "Input " << input << " (1-based) -> ";
        if (result.success) {
            std::cout << "index " << result.validated_index << " (0-based)";
        } else {
            std::cout << "INVALID";
        }
        std::cout << "\n";
    }
}

/**
 * Demonstrate interactive access
 */
void interactiveAccess() {
    std::cout << "\n--- Interactive Array Access ---\n";
    
    std::vector<std::string> fruits = {"apple", "banana", "cherry", "date", 
                                        "elderberry", "fig", "grape"};
    
    AccessConfig config;
    config.max_limit = fruits.size();
    config.max_attempts = 3;
    
    SafeArrayAccess validator(config);
    
    std::cout << "Available fruits (0-" << fruits.size() - 1 << "):\n";
    for (size_t i = 0; i < fruits.size(); ++i) {
        std::cout << "  [" << i << "] " << fruits[i] << "\n";
    }
    
    std::string value;
    auto result = validator.readAndValidate("Enter index to access: ");
    
    if (result.success) {
        std::cout << "Fruit at index " << result.original_input 
                  << ": " << fruits[result.validated_index] << "\n";
    } else {
        std::cout << "Failed to access array: " << result.error_message << "\n";
    }
}

/**
 * Demonstrate safe array wrapper
 */
void demonstrateSafeArray() {
    std::cout << "\n--- Safe Array Wrapper ---\n";
    
    SafeArray<int, 5> arr = {100, 200, 300, 400, 500};
    arr.display();
    
    // Safe access
    int value;
    auto result = arr.get(2, value);
    if (result.success) {
        std::cout << "Element at index 2: " << value << "\n";
    }
    
    // Safe modification
    result = arr.set(4, 999);
    if (result.success) {
        std::cout << "Modified element at index 4\n";
        arr.display();
    }
    
    // Out of bounds access
    result = arr.get(10, value);
    if (!result.success) {
        std::cout << "Expected error: " << result.error_message << "\n";
    }
    
    // Negative index
    result = arr.get(-1, value);
    if (!result.success) {
        std::cout << "Negative index error: " << result.error_message << "\n";
    }
}

/**
 * Demonstrate batch processing
 */
void demonstrateBatchProcessing() {
    std::cout << "\n--- Batch Validation ---\n";
    
    std::vector<int> data = {10, 20, 30, 40, 50};
    std::vector<int> indices = {2, -1, 5, 0, 3, 10, 1};
    
    AccessConfig config;
    config.max_limit = data.size();
    SafeArrayAccess validator(config);
    
    auto results = validator.validateBatch(indices);
    
    std::cout << "Batch validation results:\n";
    for (size_t i = 0; i < indices.size(); ++i) {
        std::cout << "  Index " << std::setw(2) << indices[i] << ": ";
        if (results[i].success) {
            std::cout << "OK -> data[" << results[i].validated_index 
                      << "] = " << data[results[i].validated_index];
        } else {
            std::cout << "FAIL: " << results[i].error_message;
        }
        std::cout << "\n";
    }
}

/**
 * Demonstrate range checking
 */
void demonstrateRangeChecking() {
    std::cout << "\n--- Range Checking ---\n";
    
    AccessConfig config;
    config.max_limit = 20;
    SafeArrayAccess validator(config);
    
    int start = 5, end = 15;
    std::vector<int> test_values = {3, 7, 10, 12, 18, 25};
    
    std::cout << "Checking values in range [" << start << ", " << end << "]:\n";
    for (int val : test_values) {
        bool in_range = validator.isInRange(start, end, val);
        std::cout << "  Value " << std::setw(2) << val << ": " 
                  << (in_range ? "IN range" : "OUT of range") << "\n";
    }
}

/**
 * Demonstrate exception handling
 */
void demonstrateExceptionHandling() {
    std::cout << "\n--- Exception Handling ---\n";
    
    AccessConfig config;
    config.max_limit = 5;
    config.throw_on_error = true;
    
    SafeArrayAccess validator(config);
    
    try {
        // This will throw
        auto result = validator.validateInput(10);
        std::cout << "This line won't execute\n";
    } catch (const ArrayAccessError& e) {
        std::cout << "Caught expected exception: " << e.what() << "\n";
    }
    
    try {
        // This will also throw
        auto result = validator.validateInput(-1);
        std::cout << "This line won't execute\n";
    } catch (const ArrayAccessError& e) {
        std::cout << "Caught expected exception: " << e.what() << "\n";
    }
    
    try {
        // Valid input won't throw
        auto result = validator.validateInput(3);
        std::cout << "Valid input processed successfully\n";
    } catch (const ArrayAccessError& e) {
        std::cout << "Unexpected exception: " << e.what() << "\n";
    }
}

/**
 * Main function
 */
int main() {
    try {
        std::cout << "========================================\n";
        std::cout << "Safe Array Access with MAX_LIMIT Check\n";
        std::cout << "========================================\n";
        
        demonstrateValidation();
        interactiveAccess();
        demonstrateSafeArray();
        demonstrateBatchProcessing();
        demonstrateRangeChecking();
        demonstrateExceptionHandling();
        
        // Simple function version (as requested)
        auto simpleCheck = [](int user_input, size_t MAX_LIMIT) -> bool {
            return user_input >= 0 && static_cast<size_t>(user_input) < MAX_LIMIT;
        };
        
        std::cout << "\n--- Simple check function ---\n";
        std::cout << "Input 5 with MAX_LIMIT=10: " 
                  << (simpleCheck(5, 10) ? "VALID" : "INVALID") << "\n";
        std::cout << "Input -1 with MAX_LIMIT=10: " 
                  << (simpleCheck(-1, 10) ? "VALID" : "INVALID") << "\n";
        std::cout << "Input 15 with MAX_LIMIT=10: " 
                  << (simpleCheck(15, 10) ? "VALID" : "INVALID") << "\n";
        
        std::cout << "\n=== Summary ===\n";
        std::cout << "✓ Always validate user input before array access\n";
        std::cout << "✓ Check for negative values\n";
        std::cout << "✓ Verify index is within bounds\n";
        std::cout << "✓ Provide clear error messages\n";
        std::cout << "✓ Consider retry logic for invalid input\n";
        std::cout << "✓ Use exception handling for error propagation\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}