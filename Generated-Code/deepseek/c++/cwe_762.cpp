#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include <atomic>

/**
 * Custom exception for memory errors
 */
class MemoryError : public std::runtime_error {
public:
    explicit MemoryError(const std::string& message) 
        : std::runtime_error("Memory Error: " + message) {}
};

/**
 * Memory tracker to monitor allocations/deallocations
 */
class MemoryTracker {
private:
    static inline std::atomic<size_t> total_allocated{0};
    static inline std::atomic<size_t> total_freed{0};
    static inline std::atomic<int> allocation_count{0};
    static inline std::atomic<int> new_count{0};
    static inline std::atomic<int> malloc_count{0};
    
public:
    static void record_new(size_t size) {
        total_allocated += size;
        allocation_count++;
        new_count++;
        std::cout << "[NEW] Allocated " << size << " bytes using new\n";
    }
    
    static void record_new_array(size_t size) {
        total_allocated += size;
        allocation_count++;
        new_count++;
        std::cout << "[NEW[]] Allocated " << size << " bytes using new[]\n";
    }
    
    static void record_malloc(size_t size) {
        total_allocated += size;
        allocation_count++;
        malloc_count++;
        std::cout << "[MALLOC] Allocated " << size << " bytes using malloc\n";
    }
    
    static void record_delete(size_t size) {
        total_freed += size;
        allocation_count--;
        std::cout << "[DELETE] Freed " << size << " bytes using delete\n";
    }
    
    static void record_delete_array(size_t size) {
        total_freed += size;
        allocation_count--;
        std::cout << "[DELETE[]] Freed " << size << " bytes using delete[]\n";
    }
    
    static void record_free(size_t size) {
        total_freed += size;
        allocation_count--;
        std::cout << "[FREE] Freed " << size << " bytes using free\n";
    }
    
    static void print_stats() {
        std::cout << "\n=== Memory Statistics ===\n";
        std::cout << "Total allocated: " << total_allocated << " bytes\n";
        std::cout << "Total freed: " << total_freed << " bytes\n";
        std::cout << "Current allocations: " << allocation_count << "\n";
        std::cout << "new/delete operations: " << new_count << "\n";
        std::cout << "malloc/free operations: " << malloc_count << "\n";
        
        if (total_allocated != total_freed) {
            std::cout << "⚠️  Memory leak detected: " 
                      << (total_allocated - total_freed) << " bytes not freed\n";
        }
    }
};

/**
 * Overloaded new and delete operators for tracking
 */
void* operator new(size_t size) {
    void* ptr = std::malloc(size);
    MemoryTracker::record_new(size);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    if (ptr) {
        // Can't track size easily in standard delete
        MemoryTracker::record_delete(0);
        std::free(ptr);
    }
}

void* operator new[](size_t size) {
    void* ptr = std::malloc(size);
    MemoryTracker::record_new_array(size);
    return ptr;
}

void operator delete[](void* ptr) noexcept {
    if (ptr) {
        MemoryTracker::record_delete_array(0);
        std::free(ptr);
    }
}

/**
 * Class with constructor/destructor to demonstrate object lifecycle
 */
class TestObject {
private:
    int id;
    std::string name;
    static inline std::atomic<int> next_id{1000};
    
public:
    TestObject(const std::string& n = "default") : id(++next_id), name(n) {
        std::cout << "  [Object " << id << "] Constructor: " << name << "\n";
    }
    
    ~TestObject() {
        std::cout << "  [Object " << id << "] Destructor: " << name << "\n";
    }
    
    void print() const {
        std::cout << "  [Object " << id << "] " << name << "\n";
    }
};

/**
 * WRONG: Allocate with new[], free with free()
 * THIS IS UNDEFINED BEHAVIOR - NEVER DO THIS
 */
void demonstrateWrongMallocFree() {
    std::cout << "\n=== WRONG: new[] + free() ===\n";
    std::cout << "WARNING: This demonstrates undefined behavior!\n\n";
    
    // Allocate array with new[]
    int* arr = new int[10];
    MemoryTracker::record_new_array(10 * sizeof(int));
    
    std::cout << "Allocated array of 10 ints at: " << static_cast<void*>(arr) << "\n";
    
    // Initialize array
    for (int i = 0; i < 10; ++i) {
        arr[i] = i * 10;
    }
    
    std::cout << "Array contents: ";
    for (int i = 0; i < 10; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
    
    // WRONG: Using free() instead of delete[]
    std::cout << "\nWRONG: Calling free() on memory allocated with new[]...\n";
    free(arr);  // UNDEFINED BEHAVIOR!
    MemoryTracker::record_free(10 * sizeof(int));
    
    std::cout << "Memory freed with free() - but destructors not called!\n";
    std::cout << "This is undefined behavior and may cause:\n";
    std::cout << "  - Memory corruption\n";
    std::cout << "  - Crash on some platforms\n";
    std::cout << "  - Destructors not called (for objects)\n";
}

/**
 * WRONG: Another example with objects
 */
void demonstrateWrongWithObjects() {
    std::cout << "\n=== WRONG: Object array with new[] + free() ===\n";
    
    // Allocate array of objects with new[]
    TestObject* obj_arr = new TestObject[3]{
        TestObject("First"),
        TestObject("Second"),
        TestObject("Third")
    };
    
    std::cout << "\nObjects created, using them...\n";
    for (int i = 0; i < 3; ++i) {
        obj_arr[i].print();
    }
    
    // WRONG: Using free() instead of delete[]
    std::cout << "\nWRONG: Calling free() on object array...\n";
    free(obj_arr);  // UNDEFINED BEHAVIOR - destructors NOT called!
    MemoryTracker::record_free(3 * sizeof(TestObject));
    
    std::cout << "Memory freed but destructors were not called!\n";
    std::cout << "This causes resource leaks!\n";
}

/**
 * RIGHT: Allocate with new[], delete with delete[]
 */
void demonstrateCorrectNewDelete() {
    std::cout << "\n=== CORRECT: new[] + delete[] ===\n";
    
    // Allocate array with new[]
    int* arr = new int[10];
    
    std::cout << "Allocated array at: " << static_cast<void*>(arr) << "\n";
    
    // Use array
    for (int i = 0; i < 10; ++i) {
        arr[i] = i * 10;
    }
    
    std::cout << "Array contents: ";
    for (int i = 0; i < 10; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
    
    // CORRECT: Use delete[]
    std::cout << "\nCORRECT: Calling delete[]...\n";
    delete[] arr;
    MemoryTracker::record_delete_array(10 * sizeof(int));
    
    std::cout << "Array properly deallocated with delete[]\n";
}

/**
 * RIGHT: With objects - destructors called
 */
void demonstrateCorrectWithObjects() {
    std::cout << "\n=== CORRECT: Object array with new[] + delete[] ===\n";
    
    // Allocate array of objects with new[]
    TestObject* obj_arr = new TestObject[3]{
        TestObject("Alpha"),
        TestObject("Beta"),
        TestObject("Gamma")
    };
    
    std::cout << "\nObjects created, using them...\n";
    for (int i = 0; i < 3; ++i) {
        obj_arr[i].print();
    }
    
    // CORRECT: Using delete[]
    std::cout << "\nCORRECT: Calling delete[]...\n";
    delete[] obj_arr;  // Destructors called for all objects
    MemoryTracker::record_delete_array(3 * sizeof(TestObject));
    
    std::cout << "Objects properly destroyed and memory freed\n";
}

/**
 * Alternative: malloc + free (for C-style)
 */
void demonstrateMallocFree() {
    std::cout << "\n=== C-Style: malloc + free ===\n";
    
    // Allocate with malloc
    int* arr = (int*)malloc(10 * sizeof(int));
    MemoryTracker::record_malloc(10 * sizeof(int));
    
    std::cout << "Allocated with malloc at: " << static_cast<void*>(arr) << "\n";
    
    // Use array
    for (int i = 0; i < 10; ++i) {
        arr[i] = i * 10;
    }
    
    std::cout << "Array contents: ";
    for (int i = 0; i < 10; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
    
    // Free with free
    std::cout << "\nFreeing with free()...\n";
    free(arr);
    MemoryTracker::record_free(10 * sizeof(int));
    
    std::cout << "Memory properly freed with free()\n";
}

/**
 * Modern C++: Use std::vector
 */
void demonstrateModernCpp() {
    std::cout << "\n=== MODERN C++: std::vector ===\n";
    
    {
        std::vector<int> vec;
        vec.reserve(10);
        
        for (int i = 0; i < 10; ++i) {
            vec.push_back(i * 10);
        }
        
        std::cout << "Vector contents: ";
        for (int n : vec) {
            std::cout << n << " ";
        }
        std::cout << "\n";
        
        std::cout << "Vector automatically manages memory\n";
        std::cout << "No manual new/delete needed\n";
        
    } // vector automatically freed here
    
    std::cout << "Vector went out of scope, memory automatically freed\n";
}

/**
 * Modern C++: Use std::unique_ptr for arrays
 */
void demonstrateSmartPointer() {
    std::cout << "\n=== MODERN C++: std::unique_ptr with array ===\n";
    
    {
        // C++17 style
        auto arr = std::make_unique<int[]>(10);
        
        for (int i = 0; i < 10; ++i) {
            arr[i] = i * 10;
        }
        
        std::cout << "Smart pointer array contents: ";
        for (int i = 0; i < 10; ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << "\n";
        
        std::cout << "Memory automatically managed\n";
        
    } // unique_ptr automatically calls delete[]
    
    std::cout << "Smart pointer out of scope, memory freed\n";
}

/**
 * Demonstrate the consequences
 */
void demonstrateConsequences() {
    std::cout << "\n=== Consequences of Wrong Deallocation ===\n";
    
    std::cout << "1. Undefined Behavior - program may crash\n";
    std::cout << "2. Destructors not called - resource leaks\n";
    std::cout << "3. Memory corruption - heap metadata damaged\n";
    std::cout << "4. Debug allocators may detect mismatch\n";
    std::cout << "5. Portability issues - may work on some platforms, crash on others\n";
    
    std::cout << "\nWhy it's wrong:\n";
    std::cout << "- new[] stores array count for destructor calls\n";
    std::cout << "- free() doesn't know about C++ object lifetimes\n";
    std::cout << "- Different memory management systems may be incompatible\n";
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "new[] + free() - Wrong Memory Deallocation\n";
    std::cout << "========================================\n";
    
    try {
        // Show the wrong way (for education)
        std::cout << "\nPress Enter to see WRONG examples (will demonstrate UB)...\n";
        std::cin.get();
        
        demonstrateWrongMallocFree();
        demonstrateWrongWithObjects();
        
        // Show the right ways
        std::cout << "\n\nPress Enter to see CORRECT examples...\n";
        std::cin.get();
        
        demonstrateCorrectNewDelete();
        demonstrateCorrectWithObjects();
        demonstrateMallocFree();
        demonstrateModernCpp();
        demonstrateSmartPointer();
        demonstrateConsequences();
        
        // Summary
        MemoryTracker::print_stats();
        
        std::cout << "\n=== Key Rules ===\n";
        std::cout << "✓ new → delete\n";
        std::cout << "✓ new[] → delete[]\n";
        std::cout << "✓ malloc → free\n";
        std::cout << "✓ calloc → free\n";
        std::cout << "✓ realloc → free\n";
        std::cout << "❌ NEVER mix allocation/deallocation families!\n";
        std::cout << "❌ NEVER use free() on memory from new/new[]\n";
        std::cout << "❌ NEVER use delete on memory from malloc\n";
        
        std::cout << "\n=== Best Practice ===\n";
        std::cout << "1. Use std::vector for dynamic arrays\n";
        std::cout << "2. Use std::unique_ptr<T[]> for array ownership\n";
        std::cout << "3. Avoid manual new/delete in modern C++\n";
        std::cout << "4. If you must use new, always match with delete\n";
        std::cout << "5. Never mix C and C++ allocation mechanisms\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}