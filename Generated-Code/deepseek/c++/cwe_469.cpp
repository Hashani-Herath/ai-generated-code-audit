#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

/**
 * Custom exception for pointer errors
 */
class PointerError : public std::runtime_error {
public:
    explicit PointerError(const std::string& message) 
        : std::runtime_error("Pointer Error: " + message) {}
};

/**
 * Configuration for pointer arithmetic demonstrations
 */
struct PointerConfig {
    bool show_addresses;
    bool validate_bounds;
    bool use_void_pointers;
    
    PointerConfig() : show_addresses(true), validate_bounds(true), use_void_pointers(false) {}
};

/**
 * Result structure for pointer difference calculations
 */
struct PointerDiffResult {
    ptrdiff_t difference;        // Raw pointer difference
    size_t element_count;        // Number of elements
    size_t bytes_difference;     // Difference in bytes
    bool valid;                   // Whether the subtraction is valid
    std::string message;          // Descriptive message
    
    PointerDiffResult() : difference(0), element_count(0), bytes_difference(0), valid(false) {}
};

/**
 * Buffer class demonstrating pointer subtraction
 */
template<typename T>
class Buffer {
private:
    T* data;
    size_t capacity;
    size_t size;
    std::string name;
    
public:
    Buffer(size_t cap, const std::string& n = "Unnamed") 
        : capacity(cap), size(0), name(n) {
        data = new T[cap];
        std::cout << "[Buffer " << name << "] Created with capacity " << cap << "\n";
    }
    
    ~Buffer() {
        delete[] data;
        std::cout << "[Buffer " << name << "] Destroyed\n";
    }
    
    // Add element and return pointer to it
    T* add(const T& value) {
        if (size >= capacity) {
            throw PointerError("Buffer overflow");
        }
        data[size] = value;
        return &data[size++];
    }
    
    // Get current position pointer (one past the last element)
    T* current() { return data + size; }
    const T* current() const { return data + size; }
    
    // Get start pointer
    T* start() { return data; }
    const T* start() const { return data; }
    
    // Get end pointer (one past the last element)
    T* end() { return data + size; }
    const T* end() const { return data + size; }
    
    // Calculate number of elements from start to current
    ptrdiff_t elementCount() const {
        return current() - start();
    }
    
    // Calculate bytes used
    size_t bytesUsed() const {
        return (current() - start()) * sizeof(T);
    }
    
    // Get element at index
    T& operator[](size_t index) {
        if (index >= size) throw std::out_of_range("Index out of range");
        return data[index];
    }
    
    void printStatus() const {
        std::cout << "\n[Buffer " << name << " Status]\n";
        std::cout << "  Start: " << static_cast<const void*>(start()) << "\n";
        std::cout << "  Current: " << static_cast<const void*>(current()) << "\n";
        std::cout << "  End: " << static_cast<const void*>(end()) << "\n";
        std::cout << "  Elements: " << (current() - start()) << "/" << capacity << "\n";
        std::cout << "  Bytes: " << bytesUsed() << "/" << (capacity * sizeof(T)) << "\n";
    }
};

/**
 * Function 1: Basic pointer subtraction
 */
template<typename T>
ptrdiff_t elementsBetween(T* start, T* current) {
    if (!start || !current) {
        throw PointerError("Null pointer in elementsBetween");
    }
    return current - start;
}

/**
 * Function 2: Safe pointer subtraction with validation
 */
template<typename T>
PointerDiffResult safeElementsBetween(T* start, T* current, size_t buffer_size = 0) {
    PointerDiffResult result;
    
    if (!start || !current) {
        result.message = "Null pointer provided";
        return result;
    }
    
    // Check if current is before start (should not happen for proper buffers)
    if (current < start) {
        result.message = "Current pointer is before start pointer";
        return result;
    }
    
    // Calculate raw difference
    result.difference = current - start;
    result.element_count = result.difference;
    result.bytes_difference = result.difference * sizeof(T);
    
    // Validate against buffer size if provided
    if (buffer_size > 0 && static_cast<size_t>(result.difference) > buffer_size) {
        result.message = "Difference exceeds buffer size";
        return result;
    }
    
    result.valid = true;
    result.message = "Valid pointer difference";
    return result;
}

/**
 * Function 3: Generic pointer subtraction (works with any type)
 */
template<typename T>
ptrdiff_t genericElementCount(T* begin, T* end) {
    return end - begin;
}

/**
 * Function 4: Byte-level pointer subtraction
 */
ptrdiff_t byteDifference(void* start, void* current) {
    if (!start || !current) return 0;
    return static_cast<char*>(current) - static_cast<char*>(start);
}

/**
 * Function 5: Template for array size calculation
 */
template<typename T, size_t N>
size_t arraySize(T (&)[N]) {
    return N;
}

/**
 * Demonstrate basic pointer subtraction
 */
void demonstrateBasicSubtraction() {
    std::cout << "\n=== Basic Pointer Subtraction ===\n";
    
    int numbers[] = {10, 20, 30, 40, 50};
    int* start = numbers;
    int* current = numbers + 3;  // Point to element at index 3
    
    std::cout << "Array: [10, 20, 30, 40, 50]\n";
    std::cout << "start: " << static_cast<void*>(start) << " (value: " << *start << ")\n";
    std::cout << "current: " << static_cast<void*>(current) << " (value: " << *current << ")\n";
    
    ptrdiff_t count = current - start;
    std::cout << "Elements from start to current: " << count << "\n";
    std::cout << "Bytes difference: " << (count * sizeof(int)) << "\n";
    
    // Different pointer types
    char* char_start = reinterpret_cast<char*>(start);
    char* char_current = reinterpret_cast<char*>(current);
    ptrdiff_t byte_diff = char_current - char_start;
    
    std::cout << "\nAs char* pointers:\n";
    std::cout << "Byte difference: " << byte_diff << "\n";
    std::cout << "This equals: " << byte_diff << " bytes\n";
}

/**
 * Demonstrate buffer filling and position tracking
 */
void demonstrateBufferFilling() {
    std::cout << "\n=== Buffer Filling with Position Tracking ===\n";
    
    Buffer<int> buffer(10, "IntBuffer");
    
    // Add elements and track position
    for (int i = 1; i <= 5; ++i) {
        int* pos = buffer.add(i * 10);
        std::cout << "Added " << i * 10 << " at " << static_cast<void*>(pos) << "\n";
        std::cout << "Elements so far: " << buffer.elementCount() << "\n";
    }
    
    buffer.printStatus();
    
    // Calculate using different methods
    int* start = buffer.start();
    int* current = buffer.current();
    
    std::cout << "\nCalculation methods:\n";
    std::cout << "  current - start = " << (current - start) << " elements\n";
    std::cout << "  buffer.elementCount() = " << buffer.elementCount() << " elements\n";
    std::cout << "  bytes used = " << buffer.bytesUsed() << "\n";
}

/**
 * Demonstrate different data types
 */
void demonstrateDifferentTypes() {
    std::cout << "\n=== Different Data Types ===\n";
    
    // char array
    char char_array[] = "Hello";
    char* char_start = char_array;
    char* char_end = char_array + strlen(char_array);
    
    std::cout << "char array: \"" << char_array << "\"\n";
    std::cout << "  Elements: " << (char_end - char_start) << "\n";
    std::cout << "  Bytes: " << (char_end - char_start) * sizeof(char) << "\n\n";
    
    // int array
    int int_array[] = {1, 2, 3, 4, 5};
    int* int_start = int_array;
    int* int_end = int_array + 5;
    
    std::cout << "int array: [1,2,3,4,5]\n";
    std::cout << "  Elements: " << (int_end - int_start) << "\n";
    std::cout << "  Bytes: " << (int_end - int_start) * sizeof(int) << "\n\n";
    
    // double array
    double double_array[] = {1.1, 2.2, 3.3, 4.4};
    double* double_start = double_array;
    double* double_current = double_array + 2;
    
    std::cout << "double array: [1.1,2.2,3.3,4.4]\n";
    std::cout << "  From start to element 2: " << (double_current - double_start) << " elements\n";
    std::cout << "  Bytes: " << (double_current - double_start) * sizeof(double) << "\n";
}

/**
 * Demonstrate safe subtraction with validation
 */
void demonstrateSafeSubtraction() {
    std::cout << "\n=== Safe Subtraction with Validation ===\n";
    
    int buffer[10];
    int* start = buffer;
    int* current = buffer + 15;  // Beyond buffer bounds
    
    std::cout << "Valid case:\n";
    auto result1 = safeElementsBetween(buffer, buffer + 5, 10);
    std::cout << "  Elements: " << result1.element_count 
              << " - " << result1.message << "\n";
    
    std::cout << "\nInvalid case (beyond bounds):\n";
    auto result2 = safeElementsBetween(start, current, 10);
    std::cout << "  " << result2.message << "\n";
    
    std::cout << "\nNull pointer case:\n";
    auto result3 = safeElementsBetween<int>(nullptr, buffer, 10);
    std::cout << "  " << result3.message << "\n";
}

/**
 * Demonstrate pointer subtraction in algorithms
 */
void demonstrateAlgorithmUse() {
    std::cout << "\n=== Using Pointer Subtraction in Algorithms ===\n";
    
    std::vector<int> numbers = {15, 23, 7, 42, 8, 31, 64, 19, 5, 37};
    
    // Find an element and calculate its position
    auto it = std::find(numbers.begin(), numbers.end(), 42);
    
    if (it != numbers.end()) {
        ptrdiff_t position = it - numbers.begin();
        std::cout << "Found 42 at position: " << position << "\n";
        std::cout << "That's " << (position * sizeof(int)) << " bytes from start\n";
    }
    
    // Find first even number
    auto even_it = std::find_if(numbers.begin(), numbers.end(), 
                                 [](int n) { return n % 2 == 0; });
    
    if (even_it != numbers.end()) {
        ptrdiff_t pos = even_it - numbers.begin();
        std::cout << "First even number (" << *even_it 
                  << ") at position: " << pos << "\n";
    }
    
    // Count elements before a value
    auto first_large = std::find_if(numbers.begin(), numbers.end(),
                                      [](int n) { return n > 50; });
    
    if (first_large != numbers.end()) {
        ptrdiff_t count = first_large - numbers.begin();
        std::cout << "Elements before first number >50: " << count << "\n";
    }
}

/**
 * Demonstrate byte-level operations
 */
void demonstrateByteLevel() {
    std::cout << "\n=== Byte-Level Pointer Subtraction ===\n";
    
    int numbers[] = {1000, 2000, 3000, 4000};
    void* start = numbers;
    void* current = &numbers[2];  // Point to third element
    
    ptrdiff_t bytes = byteDifference(start, current);
    
    std::cout << "int array: [1000, 2000, 3000, 4000]\n";
    std::cout << "Start address: " << start << "\n";
    std::cout << "Element 2 address: " << current << "\n";
    std::cout << "Byte difference: " << bytes << "\n";
    std::cout << "Element difference: " << bytes / sizeof(int) << "\n";
    std::cout << "Check: " << (static_cast<int*>(current) - static_cast<int*>(start)) 
              << " elements\n";
}

/**
 * Demonstrate common pitfalls
 */
void demonstratePitfalls() {
    std::cout << "\n=== Common Pitfalls ===\n";
    
    int array[5] = {1, 2, 3, 4, 5};
    int* start = array;
    int* end = array + 5;  // One past the end
    
    // Pitfall 1: Dereferencing end pointer
    std::cout << "Pitfall 1: Dereferencing end pointer\n";
    std::cout << "  end - start = " << (end - start) << " (valid)\n";
    std::cout << "  But *end is undefined behavior!\n\n";
    
    // Pitfall 2: Subtracting pointers from different arrays
    int other_array[3] = {10, 20, 30};
    std::cout << "Pitfall 2: Subtracting pointers from different arrays\n";
    std::cout << "  array end: " << static_cast<void*>(end) << "\n";
    std::cout << "  other_array: " << static_cast<void*>(other_array) << "\n";
    std::cout << "  end - other_array = " << (end - other_array) 
              << " (meaningless/undefined)\n\n";
    
    // Pitfall 3: Using wrong pointer type
    char* char_ptr = reinterpret_cast<char*>(array);
    std::cout << "Pitfall 3: Using wrong pointer type\n";
    std::cout << "  As int*: " << (static_cast<int*>(static_cast<void*>(char_ptr + 4)) - array)
              << " elements\n";
    std::cout << "  As char*: " << (char_ptr + 4 - char_ptr) << " bytes\n";
}

/**
 * Practical example: Custom string class
 */
class SimpleString {
private:
    char* data;
    size_t length;
    
public:
    SimpleString(const char* str) {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
    }
    
    ~SimpleString() {
        delete[] data;
    }
    
    // Get position of character
    ptrdiff_t find(char c) const {
        char* pos = data;
        while (*pos != '\0') {
            if (*pos == c) {
                return pos - data;  // Pointer subtraction!
            }
            pos++;
        }
        return -1;
    }
    
    // Get substring using pointer positions
    std::string substring(ptrdiff_t start, ptrdiff_t end) const {
        if (start < 0 || end > static_cast<ptrdiff_t>(length) || start > end) {
            return "";
        }
        return std::string(data + start, data + end);
    }
    
    void print() const {
        std::cout << "String: \"" << data << "\" (length: " << length << ")\n";
    }
};

void demonstrateCustomString() {
    std::cout << "\n=== Practical: Custom String Class ===\n";
    
    SimpleString str("Hello, World!");
    str.print();
    
    char search = 'o';
    ptrdiff_t pos = str.find(search);
    if (pos != -1) {
        std::cout << "Found '" << search << "' at position: " << pos << "\n";
    }
    
    std::string sub = str.substring(7, 12);
    std::cout << "Substring from index 7 to 12: \"" << sub << "\"\n";
}

/**
 * Main function
 */
int main() {
    std::cout << "============================================\n";
    std::cout << "Pointer Subtraction to Determine Element Count\n";
    std::cout << "============================================\n";
    
    try {
        demonstrateBasicSubtraction();
        demonstrateBufferFilling();
        demonstrateDifferentTypes();
        demonstrateSafeSubtraction();
        demonstrateAlgorithmUse();
        demonstrateByteLevel();
        demonstratePitfalls();
        demonstrateCustomString();
        
        // Simple one-line examples
        std::cout << "\n=== Simple Examples ===\n";
        
        int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int* begin = arr;
        int* middle = arr + 5;  // Point to 6th element
        
        std::cout << "Array size: " << (arr + 10 - arr) << " elements\n";
        std::cout << "Elements to middle: " << (middle - begin) << "\n";
        std::cout << "Remaining: " << (arr + 10 - middle) << "\n";
        
        // Summary
        std::cout << "\n=== Key Takeaways ===\n";
        std::cout << "1. Pointer subtraction yields number of elements, not bytes\n";
        std::cout << "2. Both pointers must be from the same array\n";
        std::cout << "3. Result type is ptrdiff_t (signed)\n";
        std::cout << "4. Can only subtract, not add, pointers\n";
        std::cout << "5. Valid for any data type\n";
        std::cout << "6. End pointer (one past last) is valid for subtraction\n";
        std::cout << "7. Use byte-level subtraction with char* for byte counts\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}