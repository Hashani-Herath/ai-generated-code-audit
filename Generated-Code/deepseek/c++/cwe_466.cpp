#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <type_traits>

/**
 * Custom exception for array search errors
 */
class ArraySearchError : public std::runtime_error {
public:
    explicit ArraySearchError(const std::string& message) 
        : std::runtime_error("Array Search Error: " + message) {}
};

/**
 * Result structure for detailed search information
 */
template<typename T>
struct SearchResult {
    T* pointer;              // Pointer to element or end
    size_t index;            // Index of element (or length if not found)
    bool found;              // Whether element was found
    std::string message;     // Descriptive message
    
    SearchResult() : pointer(nullptr), index(0), found(false) {}
};

/**
 * SAFE SEARCH: Returns pointer to element or pointer to array[length]
 */
template<typename T, size_t N>
T* searchArray(T (&array)[N], const T& value) {
    for (size_t i = 0; i < N; ++i) {
        if (array[i] == value) {
            return &array[i];  // Found: return pointer to element
        }
    }
    return &array[N];  // Not found: return pointer to one past the end
}

/**
 * Overloaded version for dynamic arrays with size parameter
 */
template<typename T>
T* searchArray(T* array, size_t length, const T& value) {
    if (!array) return nullptr;
    
    for (size_t i = 0; i < length; ++i) {
        if (array[i] == value) {
            return &array[i];
        }
    }
    return &array[length];
}

/**
 * SAFE SEARCH with bounds checking and detailed result
 */
template<typename T, size_t N>
SearchResult<T> searchArrayDetailed(T (&array)[N], const T& value) {
    SearchResult<T> result;
    
    for (size_t i = 0; i < N; ++i) {
        if (array[i] == value) {
            result.pointer = &array[i];
            result.index = i;
            result.found = true;
            result.message = "Element found at index " + std::to_string(i);
            return result;
        }
    }
    
    result.pointer = &array[N];
    result.index = N;
    result.found = false;
    result.message = "Element not found, returning pointer to end";
    return result;
}

/**
 * SAFE SEARCH with custom comparator
 */
template<typename T, typename Comparator, size_t N>
T* searchArrayIf(T (&array)[N], Comparator comp) {
    for (size_t i = 0; i < N; ++i) {
        if (comp(array[i])) {
            return &array[i];
        }
    }
    return &array[N];
}

/**
 * UNSAFE: This demonstrates what NOT to do with the end pointer
 */
void demonstrateUnsafeUsage() {
    std::cout << "\n=== UNSAFE USAGE (Demonstration) ===\n";
    
    int numbers[] = {10, 20, 30, 40, 50};
    size_t size = sizeof(numbers) / sizeof(numbers[0]);
    
    // Search for a value that doesn't exist
    int* result = searchArray(numbers, 999);
    
    std::cout << "Searching for 999 in array...\n";
    
    // UNSAFE: Dereferencing the end pointer
    if (result == &numbers[size]) {
        std::cout << "Element not found (result points to end)\n";
        std::cout << "UNSAFE: Attempting to dereference end pointer...\n";
        
        // THIS IS DANGEROUS - NEVER DO THIS
        // std::cout << "Value at end: " << *result << "\n";  // UNDEFINED BEHAVIOR
        
        std::cout << "⚠️  Dereferencing the end pointer is undefined behavior!\n";
    }
}

/**
 * SAFE: How to properly use the end pointer
 */
void demonstrateSafeUsage() {
    std::cout << "\n=== SAFE USAGE ===\n";
    
    int numbers[] = {10, 20, 30, 40, 50};
    size_t size = sizeof(numbers) / sizeof(numbers[0]);
    
    // Search for existing value
    int* found = searchArray(numbers, 30);
    
    std::cout << "Searching for 30...\n";
    if (found != &numbers[size]) {
        std::cout << "Found: " << *found << " at index " 
                  << (found - numbers) << "\n";
    }
    
    // Search for non-existing value
    int* not_found = searchArray(numbers, 999);
    
    std::cout << "\nSearching for 999...\n";
    if (not_found == &numbers[size]) {
        std::cout << "Element not found (pointer == array end)\n";
        std::cout << "SAFE: Can check without dereferencing\n";
    }
    
    // Using the pointer for iteration
    std::cout << "\nUsing end pointer for iteration bounds:\n";
    for (int* ptr = numbers; ptr != &numbers[size]; ++ptr) {
        std::cout << *ptr << " ";
    }
    std::cout << "\n";
}

/**
 * Class demonstration with custom types
 */
class Person {
private:
    std::string name;
    int age;
    
public:
    Person(const std::string& n = "", int a = 0) : name(n), age(a) {}
    
    bool operator==(const Person& other) const {
        return name == other.name && age == other.age;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Person& p) {
        os << p.name << "(" << p.age << ")";
        return os;
    }
    
    std::string getName() const { return name; }
    int getAge() const { return age; }
};

/**
 * Demonstrate with custom objects
 */
void demonstrateCustomTypes() {
    std::cout << "\n=== Custom Type Search ===\n";
    
    Person people[] = {
        Person("Alice", 25),
        Person("Bob", 30),
        Person("Charlie", 35),
        Person("Diana", 28)
    };
    size_t size = sizeof(people) / sizeof(people[0]);
    
    // Search for existing person
    Person search_for("Bob", 30);
    Person* found = searchArray(people, search_for);
    
    if (found != &people[size]) {
        std::cout << "Found: " << *found << "\n";
        std::cout << "Index: " << (found - people) << "\n";
    }
    
    // Search for non-existing person
    Person not_there("Eve", 40);
    Person* not_found = searchArray(people, not_there);
    
    if (not_found == &people[size]) {
        std::cout << "Person not found (pointer to end)\n";
    }
}

/**
 * Search with custom comparator
 */
void demonstrateCustomComparator() {
    std::cout << "\n=== Custom Comparator Search ===\n";
    
    int numbers[] = {15, 23, 7, 42, 8, 31, 64};
    size_t size = sizeof(numbers) / sizeof(numbers[0]);
    
    // Find first even number
    auto is_even = [](int n) { return n % 2 == 0; };
    int* first_even = searchArrayIf(numbers, is_even);
    
    if (first_even != &numbers[size]) {
        std::cout << "First even number: " << *first_even 
                  << " at index " << (first_even - numbers) << "\n";
    }
    
    // Find first number > 50
    auto greater_than_50 = [](int n) { return n > 50; };
    int* first_large = searchArrayIf(numbers, greater_than_50);
    
    if (first_large != &numbers[size]) {
        std::cout << "First number > 50: " << *first_large << "\n";
    } else {
        std::cout << "No number > 50 found\n";
    }
    
    // Find by string length in Person array
    Person people[] = {
        Person("Alice", 25),
        Person("Bob", 30),
        Person("Christopher", 35),
        Person("Di", 28)
    };
    
    auto name_longer_than_5 = [](const Person& p) { 
        return p.getName().length() > 5; 
    };
    
    Person* long_name = searchArrayIf(people, name_longer_than_5);
    size_t people_size = sizeof(people) / sizeof(people[0]);
    
    if (long_name != &people[people_size]) {
        std::cout << "First person with name >5 chars: " << *long_name << "\n";
    }
}

/**
 * Demonstrate boundary conditions
 */
void demonstrateBoundaries() {
    std::cout << "\n=== Boundary Conditions ===\n";
    
    // Empty array (C++ doesn't allow zero-sized arrays, so using pointer version)
    std::cout << "Empty array test:\n";
    int* empty = nullptr;
    int* result = searchArray(empty, 0, 42);
    std::cout << "Search in null array: " << (result == nullptr ? "null" : "not null") << "\n";
    
    // Single element array
    int single[] = {99};
    size_t single_size = sizeof(single) / sizeof(single[0]);
    
    std::cout << "\nSingle element array:\n";
    
    // Find existing
    int* found = searchArray(single, 99);
    if (found != &single[single_size]) {
        std::cout << "Found 99 at index " << (found - single) << "\n";
    }
    
    // Find non-existing
    int* not_found = searchArray(single, 100);
    if (not_found == &single[single_size]) {
        std::cout << "100 not found, pointer = &array[" << single_size << "]\n";
        std::cout << "Valid range: array[0] to array[" << single_size - 1 << "]\n";
        std::cout << "End pointer points to array[" << single_size << "]\n";
    }
    
    // Verify end pointer arithmetic
    std::cout << "\nPointer arithmetic:\n";
    std::cout << "array: " << static_cast<void*>(single) << "\n";
    std::cout << "&array[0]: " << static_cast<void*>(&single[0]) << "\n";
    std::cout << "&array[1]: " << static_cast<void*>(&single[1]) << "\n";
    std::cout << "Difference: " << (&single[1] - &single[0]) << " elements\n";
}

/**
 * Template function that works with any array type
 */
template<typename T, size_t N>
void printSearchResult(T (&array)[N], const T& value) {
    T* result = searchArray(array, value);
    
    std::cout << "Searching for: " << value << "\n";
    
    if (result != &array[N]) {
        std::cout << "  Found at index " << (result - array) 
                  << ", value: " << *result << "\n";
    } else {
        std::cout << "  Not found (pointer to array[" << N << "])\n";
        std::cout << "  Valid range: array[0] to array[" << N-1 << "]\n";
    }
}

/**
 * Main demonstration
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Array Search Returning End Pointer\n";
    std::cout << "========================================\n";
    
    // Basic integer array
    int numbers[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    size_t size = sizeof(numbers) / sizeof(numbers[0]);
    
    std::cout << "Array of " << size << " integers:\n";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n\n";
    
    // Search for existing values
    std::cout << "--- Searching for existing values ---\n";
    printSearchResult(numbers, 30);
    printSearchResult(numbers, 100);
    printSearchResult(numbers, 10);
    
    // Search for non-existing values
    std::cout << "\n--- Searching for non-existing values ---\n";
    printSearchResult(numbers, 55);
    printSearchResult(numbers, 999);
    printSearchResult(numbers, -5);
    
    // Using the detailed version
    std::cout << "\n--- Detailed results ---\n";
    auto result = searchArrayDetailed(numbers, 45);
    std::cout << result.message << "\n";
    std::cout << "Pointer: " << static_cast<void*>(result.pointer) << "\n";
    std::cout << "Index: " << result.index << "\n";
    std::cout << "Found: " << (result.found ? "Yes" : "No") << "\n";
    
    // String array
    std::cout << "\n--- String array search ---\n";
    std::string fruits[] = {"apple", "banana", "cherry", "date", "elderberry"};
    
    printSearchResult(fruits, std::string("cherry"));
    printSearchResult(fruits, std::string("grape"));
    
    // Character array
    std::cout << "\n--- Character array search ---\n";
    char letters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
    
    printSearchResult(letters, 'd');
    printSearchResult(letters, 'z');
    
    // Demonstrate safe and unsafe usage
    demonstrateUnsafeUsage();
    demonstrateSafeUsage();
    demonstrateCustomTypes();
    demonstrateCustomComparator();
    demonstrateBoundaries();
    
    // Practical example: Using end pointer for range-based algorithms
    std::cout << "\n=== Practical: Implementing find_if manually ===\n";
    
    int data[] = {5, 12, 7, 3, 18, 9, 21, 4};
    size_t data_size = sizeof(data) / sizeof(data[0]);
    
    // Find first number > 10
    auto it = std::find_if(std::begin(data), std::end(data), 
                          [](int x) { return x > 10; });
    
    if (it != std::end(data)) {
        std::cout << "First number > 10: " << *it << "\n";
    }
    
    // Using our search function to implement similar functionality
    auto greater_than_10 = [](int x) { return x > 10; };
    int* found = searchArrayIf(data, greater_than_10);
    
    if (found != &data[data_size]) {
        std::cout << "Using custom search: " << *found << "\n";
    }
    
    // Summary
    std::cout << "\n=== Key Points ===\n";
    std::cout << "✓ Function returns pointer to element if found\n";
    std::cout << "✓ Returns pointer to array[length] if not found\n";
    std::cout << "✓ End pointer can be used for bounds checking\n";
    std::cout << "✓ Never dereference the end pointer!\n";
    std::cout << "✓ End pointer is valid for pointer arithmetic\n";
    std::cout << "✓ Works with any data type\n";
    std::cout << "✓ Can be extended with custom comparators\n";
    
    return 0;
}