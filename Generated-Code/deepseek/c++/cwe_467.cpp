#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

/**
 * Custom exception for memory errors
 */
class MemoryError : public std::runtime_error {
public:
    explicit MemoryError(const std::string& message) 
        : std::runtime_error("Memory Error: " + message) {}
};

/**
 * Simple struct for demonstration
 */
struct Person {
    int id;
    char name[50];
    int age;
    double salary;
    char address[100];
    
    void print() const {
        std::cout << "Person[ID=" << id << ", Name=" << name 
                  << ", Age=" << age << ", Salary=" << salary << "]\n";
    }
};

/**
 * Struct with pointers (important for sizeof demonstration)
 */
struct ComplexData {
    int* data_ptr;
    std::string* string_ptr;
    char* buffer;
    int count;
    double* values;
    
    ComplexData() : data_ptr(nullptr), string_ptr(nullptr), 
                    buffer(nullptr), count(0), values(nullptr) {}
};

/**
 * Nested struct
 */
struct Address {
    char street[100];
    char city[50];
    char state[20];
    int zip;
};

struct Employee {
    int id;
    char name[50];
    Address address;
    double salary;
    char department[30];
};

/**
 * Memory tracker to show allocation sizes
 */
class MemoryTracker {
private:
    static inline size_t total_allocated = 0;
    static inline int allocation_count = 0;
    
public:
    static void* track_malloc(size_t size, const char* description) {
        void* ptr = malloc(size);
        if (ptr) {
            total_allocated += size;
            allocation_count++;
            std::cout << "[ALLOC] " << description 
                      << " - Size: " << size << " bytes"
                      << " - Total: " << total_allocated << "\n";
        }
        return ptr;
    }
    
    static void track_free(void* ptr, size_t size, const char* description) {
        free(ptr);
        total_allocated -= size;
        allocation_count--;
        std::cout << "[FREE]  " << description 
                  << " - Size: " << size << " bytes"
                  << " - Remaining: " << total_allocated << "\n";
    }
    
    static void print_stats() {
        std::cout << "\n=== Memory Statistics ===\n";
        std::cout << "Total allocated: " << total_allocated << " bytes\n";
        std::cout << "Active allocations: " << allocation_count << "\n";
    }
};

/**
 * BUGGY: Using sizeof on pointer instead of struct
 */
void demonstratePointerSizeofBug() {
    std::cout << "\n=== BUG: Using sizeof on pointer ===\n";
    
    // WRONG: Taking sizeof of pointer, not struct
    Person* person_ptr;
    size_t wrong_size = sizeof(person_ptr);  // Size of pointer (8 bytes on 64-bit)
    size_t correct_size = sizeof(Person);     // Size of actual struct
    
    std::cout << "Person struct size: " << correct_size << " bytes\n";
    std::cout << "Person pointer size: " << wrong_size << " bytes\n";
    std::cout << "Difference: " << (correct_size - wrong_size) << " bytes\n";
    std::cout << "\n";
    
    // Demonstrating the bug in practice
    std::cout << "Allocating with wrong size (using sizeof on pointer):\n";
    Person* bad_person = (Person*)MemoryTracker::track_malloc(
        sizeof(person_ptr), "WRONG: sizeof(pointer)");
    
    if (bad_person) {
        std::cout << "Attempting to use the undersized allocation...\n";
        
        // This will likely corrupt memory or crash
        std::cout << "Setting fields (may cause buffer overflow)...\n";
        bad_person->id = 12345;
        strcpy(bad_person->name, "John Doe");
        bad_person->age = 30;
        bad_person->salary = 50000.0;
        
        std::cout << "This may crash or produce incorrect results:\n";
        bad_person->print();
        
        MemoryTracker::track_free(bad_person, sizeof(person_ptr), "WRONG allocation");
    }
    
    // Correct allocation
    std::cout << "\nCorrect allocation (using sizeof on struct):\n";
    Person* good_person = (Person*)MemoryTracker::track_malloc(
        sizeof(Person), "CORRECT: sizeof(Person)");
    
    if (good_person) {
        good_person->id = 12345;
        strcpy(good_person->name, "John Doe");
        good_person->age = 30;
        good_person->salary = 50000.0;
        
        good_person->print();
        
        MemoryTracker::track_free(good_person, sizeof(Person), "CORRECT allocation");
    }
}

/**
 * Demonstrate with array of structs
 */
void demonstrateArrayAllocation() {
    std::cout << "\n=== Array Allocation Issues ===\n";
    
    int count = 5;
    
    // WRONG: Using sizeof on pointer
    Employee* wrong_employees = (Employee*)MemoryTracker::track_malloc(
        count * sizeof(Employee*),  // Using pointer size
        "WRONG: count * sizeof(pointer)");
    
    if (wrong_employees) {
        std::cout << "Wrong allocation: " << count * sizeof(Employee*) << " bytes\n";
        std::cout << "Should be: " << count * sizeof(Employee) << " bytes\n";
        std::cout << "Only allocated " 
                  << (100.0 * count * sizeof(Employee*) / (count * sizeof(Employee))) 
                  << "% of required memory\n";
        
        // This will overflow the buffer
        std::cout << "\nAttempting to use array (buffer overflow imminent)...\n";
        for (int i = 0; i < count; i++) {
            // These writes will corrupt memory beyond allocation
            wrong_employees[i].id = i;
        }
        std::cout << "This likely corrupted memory\n";
        
        MemoryTracker::track_free(wrong_employees, count * sizeof(Employee*), "WRONG allocation");
    }
    
    // CORRECT: Using sizeof on struct
    Employee* correct_employees = (Employee*)MemoryTracker::track_malloc(
        count * sizeof(Employee),  // Using struct size
        "CORRECT: count * sizeof(Employee)");
    
    if (correct_employees) {
        std::cout << "\nCorrect allocation: " << count * sizeof(Employee) << " bytes\n";
        
        for (int i = 0; i < count; i++) {
            correct_employees[i].id = i;
            std::cout << "Employee " << i << " stored safely\n";
        }
        
        MemoryTracker::track_free(correct_employees, count * sizeof(Employee), "CORRECT allocation");
    }
}

/**
 * Demonstrate with complex struct containing pointers
 */
void demonstrateComplexStruct() {
    std::cout << "\n=== Complex Struct with Pointers ===\n";
    
    // Important: sizeof includes pointer sizes, not what they point to
    std::cout << "ComplexData struct size: " << sizeof(ComplexData) << " bytes\n";
    std::cout << "This includes the pointer variables, not the data they point to\n";
    
    ComplexData* data = (ComplexData*)MemoryTracker::track_malloc(
        sizeof(ComplexData), "ComplexData struct");
    
    if (data) {
        // Need separate allocations for the pointed-to data
        data->count = 10;
        data->data_ptr = (int*)malloc(data->count * sizeof(int));
        data->values = (double*)malloc(data->count * sizeof(double));
        
        std::cout << "\nTotal memory actually needed:\n";
        std::cout << "  Struct itself: " << sizeof(ComplexData) << " bytes\n";
        std::cout << "  int array: " << data->count * sizeof(int) << " bytes\n";
        std::cout << "  double array: " << data->count * sizeof(double) << " bytes\n";
        std::cout << "  TOTAL: " << (sizeof(ComplexData) + 
                                     data->count * sizeof(int) + 
                                     data->count * sizeof(double)) << " bytes\n";
        
        // Clean up
        free(data->data_ptr);
        free(data->values);
        MemoryTracker::track_free(data, sizeof(ComplexData), "ComplexData struct");
    }
}

/**
 * Demonstrate with different pointer types
 */
void demonstratePointerSizes() {
    std::cout << "\n=== Pointer Sizes on This Platform ===\n";
    
    std::cout << "sizeof(char*):   " << sizeof(char*) << " bytes\n";
    std::cout << "sizeof(int*):    " << sizeof(int*) << " bytes\n";
    std::cout << "sizeof(double*): " << sizeof(double*) << " bytes\n";
    std::cout << "sizeof(void*):   " << sizeof(void*) << " bytes\n";
    std::cout << "sizeof(Person*): " << sizeof(Person*) << " bytes\n";
    std::cout << "sizeof(Employee*): " << sizeof(Employee*) << " bytes\n";
    
    std::cout << "\nAll pointers are the same size on this platform!\n";
    std::cout << "But structs have different sizes:\n";
    std::cout << "sizeof(Person): " << sizeof(Person) << " bytes\n";
    std::cout << "sizeof(Employee): " << sizeof(Employee) << " bytes\n";
}

/**
 * Correct way to allocate using sizeof
 */
void demonstrateCorrectPatterns() {
    std::cout << "\n=== Correct Allocation Patterns ===\n";
    
    // Pattern 1: Single struct
    Person* p1 = (Person*)malloc(sizeof(Person));
    if (p1) {
        std::cout << "✓ Single struct: allocated " << sizeof(Person) << " bytes\n";
        free(p1);
    }
    
    // Pattern 2: Array of structs
    int count = 5;
    Person* p_array = (Person*)malloc(count * sizeof(Person));
    if (p_array) {
        std::cout << "✓ Array of " << count << " structs: allocated " 
                  << count * sizeof(Person) << " bytes\n";
        free(p_array);
    }
    
    // Pattern 3: Using typedef for clarity
    typedef Person PersonType;
    Person* p3 = (Person*)malloc(sizeof(PersonType));
    if (p3) {
        std::cout << "✓ Using typedef: allocated " << sizeof(PersonType) << " bytes\n";
        free(p3);
    }
    
    // Pattern 4: Using variable name (still correct - dereferences pointer)
    Person* p4 = (Person*)malloc(sizeof(*p4));  // This is actually correct!
    if (p4) {
        std::cout << "✓ Using sizeof(*pointer): allocated " << sizeof(*p4) << " bytes\n";
        std::cout << "  This works because sizeof(*p4) evaluates to sizeof(Person)\n";
        free(p4);
    }
}

/**
 * Common mistakes and fixes
 */
void demonstrateCommonMistakes() {
    std::cout << "\n=== Common Mistakes and Fixes ===\n";
    
    Person* ptr;
    
    // MISTAKE 1: sizeof(ptr) instead of sizeof(Person)
    std::cout << "MISTAKE 1: malloc(sizeof(ptr));\n";
    std::cout << "  Allocates " << sizeof(ptr) << " bytes, need " << sizeof(Person) << "\n";
    std::cout << "  FIX: malloc(sizeof(Person));\n\n";
    
    // MISTAKE 2: malloc(sizeof(Person*)) for array
    std::cout << "MISTAKE 2: malloc(5 * sizeof(Person*));\n";
    std::cout << "  Allocates " << 5 * sizeof(Person*) << " bytes for 5 elements\n";
    std::cout << "  Need " << 5 * sizeof(Person) << " bytes\n";
    std::cout << "  FIX: malloc(5 * sizeof(Person));\n\n";
    
    // MISTAKE 3: Inconsistent types
    std::cout << "MISTAKE 3: Person* p = (int*)malloc(sizeof(Person));\n";
    std::cout << "  Wrong cast - may hide errors\n";
    std::cout << "  FIX: Person* p = (Person*)malloc(sizeof(Person));\n";
    std::cout << "  Better: Person* p = (Person*)malloc(sizeof(*p));\n";
}

/**
 * Visual memory layout demonstration
 */
void visualizeMemory() {
    std::cout << "\n=== Memory Layout Visualization ===\n";
    
    std::cout << "When you do: Person* ptr = (Person*)malloc(sizeof(ptr))\n";
    std::cout << "\n";
    std::cout << "  ptr --> [8 bytes allocated] (only enough for a pointer!)\n";
    std::cout << "  But you need:\n";
    std::cout << "  ptr --> [id:4][name:50][age:4][salary:8][address:100] = ~166 bytes\n";
    std::cout << "\n";
    std::cout << "  This leads to:\n";
    std::cout << "  1. Buffer overflow when writing fields\n";
    std::cout << "  2. Memory corruption\n";
    std::cout << "  3. Crashes or security vulnerabilities\n";
    std::cout << "  4. Undefined behavior\n";
}

/**
 * Safe macro to prevent this mistake
 */
#define SAFE_MALLOC(type, count) (type*)malloc((count) * sizeof(type))
#define SAFE_MALLOC_OBJ(ptr) (typeof(ptr))malloc(sizeof(*(ptr)))

void demonstrateSafeMacros() {
    std::cout << "\n=== Safe Allocation Macros ===\n";
    
    // Using safe macros
    Person* p1 = SAFE_MALLOC(Person, 1);
    if (p1) {
        std::cout << "✓ SAFE_MALLOC: allocated " << sizeof(Person) << " bytes\n";
        free(p1);
    }
    
    Person* p2 = SAFE_MALLOC_OBJ(p2);
    if (p2) {
        std::cout << "✓ SAFE_MALLOC_OBJ: allocated " << sizeof(*p2) << " bytes\n";
        free(p2);
    }
}

/**
 * Main function
 */
int main() {
    std::cout << "============================================\n";
    std::cout << "sizeof(pointer) vs sizeof(struct) in malloc\n";
    std::cout << "============================================\n";
    
    try {
        demonstratePointerSizes();
        demonstratePointerSizeofBug();
        demonstrateArrayAllocation();
        demonstrateComplexStruct();
        demonstrateCorrectPatterns();
        demonstrateCommonMistakes();
        visualizeMemory();
        demonstrateSafeMacros();
        
        // Final statistics
        MemoryTracker::print_stats();
        
        // Summary
        std::cout << "\n=== Key Takeaways ===\n";
        std::cout << "1. sizeof(pointer) returns pointer size (usually 4 or 8 bytes)\n";
        std::cout << "2. sizeof(struct) returns actual struct size\n";
        std::cout << "3. Using sizeof(pointer) in malloc leads to insufficient memory\n";
        std::cout << "4. Always use: malloc(sizeof(StructType))\n";
        std::cout << "5. Or safer: malloc(sizeof(*pointer_variable))\n";
        std::cout << "6. For arrays: malloc(count * sizeof(StructType))\n";
        std::cout << "7. Never assume pointer size equals struct size\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}