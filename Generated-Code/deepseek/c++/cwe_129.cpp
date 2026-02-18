#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <random>

/**
 * Custom exception for vector indexing errors
 */
class VectorIndexError : public std::runtime_error {
public:
    explicit VectorIndexError(const std::string& message) 
        : std::runtime_error("Vector Index Error: " + message) {}
};

/**
 * Configuration for vector indexing operations
 */
struct IndexingConfig {
    bool allow_negative_index;      // Allow negative indices (Python-style)
    bool use_one_based;             // Use 1-based indexing for user input
    bool check_bounds;               // Always check bounds
    bool allow_modification;         // Allow modifying vector elements
    size_t max_attempts;              // Maximum input attempts
    int default_value;                // Default value for new elements
    
    IndexingConfig() 
        : allow_negative_index(false)
        , use_one_based(false)
        , check_bounds(true)
        , allow_modification(true)
        , max_attempts(3)
        , default_value(0) {}
};

/**
 * Result structure for indexing operations
 */
template<typename T>
struct IndexResult {
    bool success;
    size_t actual_index;          // Actual vector index used
    int user_input;                // Original user input
    T value;                       // Value at index (if read)
    bool out_of_bounds;            // Whether index was out of bounds
    bool negative_handled;         // Whether negative index was handled
    std::string error_message;
    
    IndexResult() : success(false), actual_index(0), user_input(0),
                    value(T()), out_of_bounds(false), negative_handled(false) {}
};

/**
 * Safe vector wrapper with input validation
 */
template<typename T>
class SafeVector {
private:
    std::vector<T> data;
    IndexingConfig config;
    
    /**
     * Convert user input to actual vector index
     */
    size_t convertToIndex(int user_input, bool& negative_handled) const {
        negative_handled = false;
        
        if (user_input >= 0) {
            // Positive index
            if (config.use_one_based) {
                return static_cast<size_t>(user_input - 1);
            } else {
                return static_cast<size_t>(user_input);
            }
        } else {
            // Negative index (Python-style: -1 = last element)
            negative_handled = config.allow_negative_index;
            if (config.allow_negative_index) {
                // Convert negative to positive: -1 -> size-1, -2 -> size-2, etc.
                return static_cast<size_t>(static_cast<int>(data.size()) + user_input);
            } else {
                return 0; // Will be caught by bounds check
            }
        }
    }
    
    /**
     * Validate that index is within bounds
     */
    bool isIndexValid(size_t index) const {
        return index < data.size();
    }
    
    /**
     * Parse integer from string with validation
     */
    bool parseInt(const std::string& input, int& result) const {
        if (input.empty()) return false;
        
        // Check for valid integer format
        size_t pos = 0;
        try {
            result = std::stoi(input, &pos);
            
            // Check if entire string was consumed
            if (pos != input.length()) {
                return false;
            }
            
            // Check for leading zeros (optional)
            if (input.length() > 1 && input[0] == '0') {
                // Allow but warn? We'll accept for now
            }
            
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
public:
    // Constructors
    SafeVector() : data() {}
    
    explicit SafeVector(const IndexingConfig& cfg) : data(), config(cfg) {}
    
    explicit SafeVector(size_t initial_size, const IndexingConfig& cfg = IndexingConfig{}) 
        : data(initial_size), config(cfg) {
        // Initialize with default values
        if (config.default_value != 0) {
            std::fill(data.begin(), data.end(), config.default_value);
        }
    }
    
    SafeVector(std::initializer_list<T> init, const IndexingConfig& cfg = IndexingConfig{})
        : data(init), config(cfg) {}
    
    // Add element to vector
    void push_back(const T& value) {
        data.push_back(value);
    }
    
    // Get vector size
    size_t size() const {
        return data.size();
    }
    
    // Check if vector is empty
    bool empty() const {
        return data.empty();
    }
    
    /**
     * Method 1: Read index and access element with validation
     */
    IndexResult<T> accessByUserInput(const std::string& prompt = "Enter index: ") {
        IndexResult<T> result;
        
        std::cout << prompt;
        
        std::string input;
        std::getline(std::cin, input);
        
        // Parse input
        int user_index;
        if (!parseInt(input, user_index)) {
            result.error_message = "Invalid number format";
            return result;
        }
        
        result.user_input = user_index;
        
        // Convert to actual index
        size_t actual_index = convertToIndex(user_index, result.negative_handled);
        result.actual_index = actual_index;
        
        // Check bounds
        if (!isIndexValid(actual_index)) {
            result.out_of_bounds = true;
            result.error_message = "Index " + std::to_string(user_index) + 
                                   " is out of bounds (vector size: " + 
                                   std::to_string(data.size()) + ")";
            return result;
        }
        
        // Access the element
        result.value = data[actual_index];
        result.success = true;
        
        return result;
    }
    
    /**
     * Method 2: Read index and modify element
     */
    IndexResult<T> modifyByUserInput(const T& new_value, 
                                      const std::string& prompt = "Enter index to modify: ") {
        IndexResult<T> result;
        
        if (!config.allow_modification) {
            result.error_message = "Modification not allowed by configuration";
            return result;
        }
        
        auto access_result = accessByUserInput(prompt);
        
        if (access_result.success) {
            data[access_result.actual_index] = new_value;
            result = access_result;
            result.value = new_value;  // Update to show new value
        } else {
            result = access_result;
        }
        
        return result;
    }
    
    /**
     * Method 3: Multiple indices input
     */
    std::vector<IndexResult<T>> accessMultiple(const std::string& prompt = "Enter indices (space-separated): ") {
        std::vector<IndexResult<T>> results;
        
        std::cout << prompt;
        
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        
        std::string token;
        while (iss >> token) {
            IndexResult<T> result;
            
            int user_index;
            if (!parseInt(token, user_index)) {
                result.error_message = "Invalid number: " + token;
                results.push_back(result);
                continue;
            }
            
            result.user_input = user_index;
            
            size_t actual_index = convertToIndex(user_index, result.negative_handled);
            result.actual_index = actual_index;
            
            if (!isIndexValid(actual_index)) {
                result.out_of_bounds = true;
                result.error_message = "Index " + std::to_string(user_index) + " out of bounds";
            } else {
                result.value = data[actual_index];
                result.success = true;
            }
            
            results.push_back(result);
        }
        
        return results;
    }
    
    /**
     * Method 4: Range-based access with validation
     */
    bool accessRange(int start, int end, std::vector<T>& output) {
        output.clear();
        
        bool neg_start_handled, neg_end_handled;
        size_t actual_start = convertToIndex(start, neg_start_handled);
        size_t actual_end = convertToIndex(end, neg_end_handled);
        
        // Validate range
        if (!isIndexValid(actual_start) || !isIndexValid(actual_end)) {
            return false;
        }
        
        if (actual_start > actual_end) {
            std::swap(actual_start, actual_end);
        }
        
        for (size_t i = actual_start; i <= actual_end && i < data.size(); ++i) {
            output.push_back(data[i]);
        }
        
        return true;
    }
    
    /**
     * Method 5: Interactive explorer mode
     */
    void explorerMode() {
        std::cout << "\n=== Vector Explorer Mode ===\n";
        std::cout << "Commands: i <index> (inspect), m <index> <value> (modify),\n";
        std::cout << "          s (show size), a (show all), q (quit)\n\n";
        
        while (true) {
            std::cout << "> ";
            
            std::string line;
            std::getline(std::cin, line);
            
            if (line.empty()) continue;
            
            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;
            
            if (cmd == "q" || cmd == "quit") {
                break;
            }
            else if (cmd == "s" || cmd == "size") {
                std::cout << "Vector size: " << data.size() << "\n";
            }
            else if (cmd == "a" || cmd == "all") {
                std::cout << "Vector contents: ";
                for (size_t i = 0; i < data.size(); ++i) {
                    std::cout << "[" << i << "]=" << data[i] << " ";
                }
                std::cout << "\n";
            }
            else if (cmd == "i" || cmd == "inspect") {
                int idx;
                if (iss >> idx) {
                    auto result = accessByUserInput(""); // Already have input
                    if (result.success) {
                        std::cout << "data[" << idx << "] = " << result.value << "\n";
                        if (result.negative_handled) {
                            std::cout << "  (negative index handled: actual index " 
                                      << result.actual_index << ")\n";
                        }
                    } else {
                        std::cout << "Error: " << result.error_message << "\n";
                    }
                } else {
                    std::cout << "Usage: i <index>\n";
                }
            }
            else if (cmd == "m" || cmd == "modify") {
                int idx;
                T val;
                if (iss >> idx >> val) {
                    auto result = modifyByUserInput(val, "");
                    if (result.success) {
                        std::cout << "Modified: data[" << idx << "] = " << val << "\n";
                    } else {
                        std::cout << "Error: " << result.error_message << "\n";
                    }
                } else {
                    std::cout << "Usage: m <index> <value>\n";
                }
            }
            else {
                std::cout << "Unknown command. Available: i, m, s, a, q\n";
            }
        }
    }
    
    /**
     * Print vector contents
     */
    void print() const {
        std::cout << "Vector (size=" << data.size() << "): [";
        for (size_t i = 0; i < data.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << data[i];
        }
        std::cout << "]\n";
    }
    
    // Get raw vector (for compatibility)
    const std::vector<T>& getVector() const { return data; }
};

/**
 * Utility function to print index results
 */
template<typename T>
void printIndexResult(const IndexResult<T>& result) {
    std::cout << "  Success: " << (result.success ? "✓" : "✗") << "\n";
    
    if (result.success) {
        std::cout << "  User input: " << result.user_input << "\n";
        std::cout << "  Actual index: " << result.actual_index << "\n";
        std::cout << "  Value: " << result.value << "\n";
        if (result.negative_handled) {
            std::cout << "  (Negative index handled)\n";
        }
    } else {
        std::cout << "  Error: " << result.error_message << "\n";
        if (result.out_of_bounds) {
            std::cout << "  (Out of bounds)\n";
        }
    }
}

/**
 * Demonstrate various indexing techniques
 */
void demonstrateIndexing() {
    std::cout << "Vector Indexing with User Input\n";
    std::cout << "===============================\n";
    
    // Create a test vector
    SafeVector<int> vec({10, 20, 30, 40, 50, 60, 70, 80, 90, 100});
    
    std::cout << "Initial vector: ";
    vec.print();
    std::cout << "\n";
    
    // Example 1: Basic indexing
    {
        std::cout << "--- Example 1: Basic indexing ---\n";
        auto result = vec.accessByUserInput("Enter an index (0-9): ");
        printIndexResult(result);
        std::cout << "\n";
    }
    
    // Example 2: With negative indices allowed
    {
        std::cout << "--- Example 2: Negative indices allowed ---\n";
        IndexingConfig cfg;
        cfg.allow_negative_index = true;
        
        SafeVector<int> vec_neg({10, 20, 30, 40, 50}, cfg);
        vec_neg.print();
        
        auto result = vec_neg.accessByUserInput("Enter index (-5 to -1 for reverse): ");
        printIndexResult(result);
        std::cout << "\n";
    }
    
    // Example 3: 1-based indexing for users
    {
        std::cout << "--- Example 3: 1-based indexing ---\n";
        IndexingConfig cfg;
        cfg.use_one_based = true;
        
        SafeVector<int> vec_one({100, 200, 300, 400, 500}, cfg);
        vec_one.print();
        
        auto result = vec_one.accessByUserInput("Enter index (1-5): ");
        printIndexResult(result);
        std::cout << "\n";
    }
    
    // Example 4: Multiple indices
    {
        std::cout << "--- Example 4: Multiple indices ---\n";
        SafeVector<int> vec_multi({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
        vec_multi.print();
        
        auto results = vec_multi.accessMultiple("Enter multiple indices (space-separated): ");
        
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "Result " << i + 1 << ":\n";
            printIndexResult(results[i]);
        }
        std::cout << "\n";
    }
    
    // Example 5: Range access
    {
        std::cout << "--- Example 5: Range access ---\n";
        SafeVector<int> vec_range({5, 10, 15, 20, 25, 30, 35, 40});
        vec_range.print();
        
        int start, end;
        std::cout << "Enter start index: ";
        std::cin >> start;
        std::cout << "Enter end index: ";
        std::cin >> end;
        std::cin.ignore();
        
        std::vector<int> range_values;
        if (vec_range.accessRange(start, end, range_values)) {
            std::cout << "Range [" << start << ":" << end << "] = [";
            for (size_t i = 0; i < range_values.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << range_values[i];
            }
            std::cout << "]\n";
        } else {
            std::cout << "Invalid range\n";
        }
        std::cout << "\n";
    }
    
    // Example 6: Modification
    {
        std::cout << "--- Example 6: Modification ---\n";
        SafeVector<int> vec_mod({100, 200, 300, 400, 500});
        vec_mod.print();
        
        auto result = vec_mod.modifyByUserInput(999, "Enter index to modify: ");
        
        if (result.success) {
            std::cout << "Modified successfully!\n";
            vec_mod.print();
        } else {
            std::cout << "Modification failed: " << result.error_message << "\n";
        }
        std::cout << "\n";
    }
}

/**
 * Demonstrate error handling
 */
void demonstrateErrorHandling() {
    std::cout << "\nError Handling Demonstration\n";
    std::cout << "============================\n";
    
    SafeVector<int> vec({1, 2, 3});
    vec.print();
    
    // Test various error cases
    std::vector<std::string> test_inputs = {
        "5",      // Out of bounds
        "-1",     // Negative without allowing
        "abc",    // Non-numeric
        "3.14",   // Float
        "",       // Empty
        "9999999999999999"  // Overflow
    };
    
    for (const auto& input : test_inputs) {
        std::cout << "\nTesting input: \"" << input << "\"\n";
        
        // We need to feed the input manually
        std::streambuf* old_cin = std::cin.rdbuf();
        std::istringstream ss(input + "\n");
        std::cin.rdbuf(ss.rdbuf());
        
        auto result = vec.accessByUserInput("");
        printIndexResult(result);
        
        std::cin.rdbuf(old_cin);  // Restore
    }
}

/**
 * Demonstrate interactive explorer
 */
void demonstrateExplorer() {
    std::cout << "\nInteractive Explorer Mode\n";
    std::cout << "==========================\n";
    
    // Create vector with sample data
    SafeVector<std::string> vec;
    vec.push_back("apple");
    vec.push_back("banana");
    vec.push_back("cherry");
    vec.push_back("date");
    vec.push_back("elderberry");
    
    std::cout << "Starting with vector: ";
    vec.print();
    
    vec.explorerMode();
}

/**
 * Practical example: Student grades
 */
void studentGradesExample() {
    std::cout << "\nPractical Example: Student Grades\n";
    std::cout << "==================================\n";
    
    std::vector<int> grades = {85, 92, 78, 95, 88, 76, 89, 94, 82, 91};
    SafeVector<int> grade_vector(grades);
    
    std::cout << "Class grades:\n";
    for (size_t i = 0; i < grade_vector.size(); ++i) {
        std::cout << "  Student " << i + 1 << ": " << grade_vector.getVector()[i] << "\n";
    }
    
    // Teacher queries
    std::cout << "\nTeacher: Enter student number to see grade (1-10): ";
    int student_num;
    std::cin >> student_num;
    std::cin.ignore();
    
    IndexingConfig cfg;
    cfg.use_one_based = true;
    SafeVector<int> query_vector(grades, cfg);
    
    auto result = query_vector.accessByUserInput("");
    if (result.success) {
        std::cout << "Student " << student_num << " grade: " << result.value << "\n";
    } else {
        std::cout << "Invalid student number!\n";
    }
}

/**
 * Main function
 */
int main() {
    try {
        int choice;
        
        do {
            std::cout << "\n=== Vector Indexing Demonstration ===\n";
            std::cout << "1. Basic indexing examples\n";
            std::cout << "2. Error handling demonstration\n";
            std::cout << "3. Interactive explorer mode\n";
            std::cout << "4. Student grades example\n";
            std::cout << "5. Custom indexing test\n";
            std::cout << "0. Exit\n";
            std::cout << "Choice: ";
            
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1:
                    demonstrateIndexing();
                    break;
                case 2:
                    demonstrateErrorHandling();
                    break;
                case 3:
                    demonstrateExplorer();
                    break;
                case 4:
                    studentGradesExample();
                    break;
                case 5: {
                    std::cout << "\nCustom Indexing Test\n";
                    std::cout << "Enter vector size: ";
                    size_t size;
                    std::cin >> size;
                    std::cin.ignore();
                    
                    SafeVector<int> custom_vec(size);
                    
                    // Fill with some values
                    for (size_t i = 0; i < size; ++i) {
                        custom_vec.getVector()[i] = static_cast<int>(i * 10);
                    }
                    
                    custom_vec.print();
                    
                    std::cout << "Test some indices (enter 'q' to quit):\n";
                    while (true) {
                        std::cout << "Index: ";
                        std::string input;
                        std::getline(std::cin, input);
                        
                        if (input == "q") break;
                        
                        // Feed the input
                        std::streambuf* old_cin = std::cin.rdbuf();
                        std::istringstream ss(input + "\n");
                        std::cin.rdbuf(ss.rdbuf());
                        
                        auto result = custom_vec.accessByUserInput("");
                        printIndexResult(result);
                        
                        std::cin.rdbuf(old_cin);
                    }
                    break;
                }
                case 0:
                    std::cout << "Goodbye!\n";
                    break;
                default:
                    std::cout << "Invalid choice\n";
            }
            
        } while (choice != 0);
        
    } catch (const VectorIndexError& e) {
        std::cerr << "Vector indexing error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}