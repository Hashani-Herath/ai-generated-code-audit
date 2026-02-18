#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

/**
 * Custom exception for dangling pointer errors
 */
class DanglingPointerError : public std::runtime_error {
public:
    explicit DanglingPointerError(const std::string& message) 
        : std::runtime_error("Dangling Pointer Error: " + message) {}
};

/**
 * Global list to store pointers (DANGEROUS!)
 */
std::vector<void*> global_pointer_list;
std::vector<std::string> pointer_descriptions;

/**
 * Logger for tracking pointer operations
 */
class PointerLogger {
private:
    static inline int next_id = 1000;
    
public:
    static void log_addition(void* ptr, const std::string& description) {
        std::cout << "[LOG] Adding pointer " << ptr 
                  << " to global list: " << description << "\n";
        global_pointer_list.push_back(ptr);
        pointer_descriptions.push_back(description);
    }
    
    static void log_access(size_t index) {
        if (index < global_pointer_list.size()) {
            std::cout << "[LOG] Accessing pointer at index " << index 
                      << ": " << global_pointer_list[index] 
                      << " (" << pointer_descriptions[index] << ")\n";
        }
    }
    
    static void print_list() {
        std::cout << "\n=== Global Pointer List ===\n";
        for (size_t i = 0; i < global_pointer_list.size(); ++i) {
            std::cout << "[" << i << "] " << global_pointer_list[i]
                      << " - " << pointer_descriptions[i] << "\n";
        }
    }
};

/**
 * Simple class to demonstrate object lifecycle
 */
class DataObject {
private:
    int id;
    std::string name;
    std::string data;
    static inline int next_id = 1;
    
public:
    DataObject(const std::string& n, const std::string& d) 
        : id(next_id++), name(n), data(d) {
        std::cout << "  [Object " << id << "] Created: " << name << " at " 
                  << static_cast<void*>(this) << "\n";
    }
    
    ~DataObject() {
        std::cout << "  [Object " << id << "] Destroyed: " << name 
                  << " (memory at " << static_cast<void*>(this) << ")\n";
        // Invalidate data to help detect use-after-free
        id = -1;
        name = "[FREED]";
        data = "[FREED]";
    }
    
    void print() const {
        std::cout << "  [Object " << id << "] " << name << ": " << data << "\n";
    }
    
    void update(const std::string& new_data) {
        data = new_data;
    }
    
    int get_id() const { return id; }
};

/**
 * DANGEROUS: Creates local object and stores pointer in global list
 */
void dangerousFunction1() {
    std::cout << "\n--- Entering dangerousFunction1 ---\n";
    
    // Local object (will be destroyed when function exits)
    DataObject local_obj("Local1", "Sensitive data");
    
    // DANGEROUS: Storing pointer to local object in global list
    PointerLogger::log_addition(&local_obj, "Local object from dangerousFunction1");
    
    // Use object while still alive
    std::cout << "Using object within function:\n";
    local_obj.print();
    
    std::cout << "--- Exiting dangerousFunction1 (local_obj will be destroyed) ---\n";
}

/**
 * Another dangerous function with different local object
 */
void dangerousFunction2() {
    std::cout << "\n--- Entering dangerousFunction2 ---\n";
    
    // Another local object
    DataObject local_obj("Local2", "Secret data: 12345");
    
    // DANGEROUS: Storing another pointer
    PointerLogger::log_addition(&local_obj, "Local object from dangerousFunction2");
    
    local_obj.print();
    
    std::cout << "--- Exiting dangerousFunction2 ---\n";
}

/**
 * DANGEROUS: Function that returns pointer to local (also bad)
 */
DataObject* dangerousFunction3() {
    std::cout << "\n--- Entering dangerousFunction3 ---\n";
    
    DataObject local_obj("Local3", "Returned data");
    
    // DANGEROUS: Returning pointer to local
    std::cout << "--- Exiting dangerousFunction3 (returning pointer to destroyed object) ---\n";
    
    return &local_obj;  // Dangling pointer!
}

/**
 * DANGEROUS: Function that stores pointer to local array
 */
void dangerousArrayFunction() {
    std::cout << "\n--- Entering dangerousArrayFunction ---\n";
    
    int local_array[5] = {10, 20, 30, 40, 50};
    
    PointerLogger::log_addition(local_array, "Local array from dangerousArrayFunction");
    
    std::cout << "Local array contents: ";
    for (int i : local_array) std::cout << i << " ";
    std::cout << "\n";
    
    std::cout << "--- Exiting dangerousArrayFunction ---\n";
}

/**
 * Function that attempts to use dangling pointers
 */
void useDanglingPointers() {
    std::cout << "\n=== Using Dangling Pointers (Undefined Behavior!) ===\n";
    
    for (size_t i = 0; i < global_pointer_list.size(); ++i) {
        PointerLogger::log_access(i);
        
        void* ptr = global_pointer_list[i];
        std::cout << "Attempting to use pointer as DataObject*...\n";
        
        // Cast to DataObject* (dangerous!)
        DataObject* obj = static_cast<DataObject*>(ptr);
        
        // Try to use the object (UNDEFINED BEHAVIOR!)
        std::cout << "  Object ID: " << obj->get_id() << "\n";  // May crash or print garbage
        
        // Try to print (likely crash or garbage)
        obj->print();  // UNDEFINED BEHAVIOR!
        
        std::cout << "  ⚠️  If we got here, we got lucky - but it's still UB!\n";
    }
}

/**
 * Safe approach: Use smart pointers and weak pointers
 */
class SafePointerManager {
private:
    static inline std::vector<std::weak_ptr<DataObject>> safe_pointer_list;
    static inline std::vector<std::string> descriptions;
    
public:
    static void add_pointer(const std::shared_ptr<DataObject>& ptr, 
                            const std::string& desc) {
        safe_pointer_list.push_back(ptr);
        descriptions.push_back(desc);
        std::cout << "[SAFE] Added weak pointer: " << desc << "\n";
    }
    
    static void use_safe_pointers() {
        std::cout << "\n=== Using Safe Weak Pointers ===\n";
        
        for (size_t i = 0; i < safe_pointer_list.size(); ++i) {
            if (auto ptr = safe_pointer_list[i].lock()) {
                std::cout << "Pointer " << i << " (" << descriptions[i] << ") is valid:\n";
                ptr->print();
            } else {
                std::cout << "Pointer " << i << " (" << descriptions[i] << ") has expired\n";
            }
        }
    }
};

/**
 * Safe function using shared_ptr
 */
void safeFunction() {
    std::cout << "\n--- Entering safeFunction ---\n";
    
    auto obj = std::make_shared<DataObject>("SafeObj", "This object lives on heap");
    
    // Store weak pointer in global list
    SafePointerManager::add_pointer(obj, "Heap object from safeFunction");
    
    obj->print();
    
    std::cout << "--- Exiting safeFunction (object still alive on heap) ---\n";
}

/**
 * Clean up global list (for demonstration)
 */
void cleanup() {
    std::cout << "\n=== Cleaning Up ===\n";
    global_pointer_list.clear();
    pointer_descriptions.clear();
}

/**
 * Main demonstration
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Dangling Pointers: Local Objects in Global List\n";
    std::cout << "========================================\n";
    
    try {
        // First, demonstrate the dangerous behavior
        std::cout << "\n=== PART 1: Creating Dangling Pointers ===\n";
        
        dangerousFunction1();  // Stores pointer to local, then local dies
        dangerousFunction2();  // Stores another pointer to different local
        dangerousArrayFunction();  // Stores pointer to local array
        
        // This one returns a dangling pointer directly
        DataObject* dangling = dangerousFunction3();
        if (dangling) {
            PointerLogger::log_addition(dangling, "Returned pointer from dangerousFunction3");
        }
        
        // Show what's in the global list
        PointerLogger::print_list();
        
        // Now try to use the dangling pointers
        std::cout << "\n=== PART 2: Using Dangling Pointers ===\n";
        std::cout << "Press Enter to attempt using dangling pointers (will likely crash)...\n";
        std::cin.get();
        
        // This will likely crash or produce garbage
        useDanglingPointers();
        
        // Clean up the dangerous list
        cleanup();
        
        // Now demonstrate the safe approach
        std::cout << "\n=== PART 3: Safe Approach with Weak Pointers ===\n";
        
        // Create a scope for the safe approach
        {
            auto heap_obj1 = std::make_shared<DataObject>("Heap1", "Lives in scope");
            auto heap_obj2 = std::make_shared<DataObject>("Heap2", "Also lives in scope");
            
            SafePointerManager::add_pointer(heap_obj1, "First heap object");
            SafePointerManager::add_pointer(heap_obj2, "Second heap object");
            
            // These will be valid within the scope
            SafePointerManager::use_safe_pointers();
            
            // Call safe function that adds its own pointer
            safeFunction();
            
        } // heap_obj1 and heap_obj2 destroyed here
        
        // Now check which pointers are still valid
        std::cout << "\n=== After scope exit ===\n";
        SafePointerManager::use_safe_pointers();
        
        // Summary
        std::cout << "\n=== Summary ===\n";
        std::cout << "❌ NEVER store pointers to local objects globally\n";
        std::cout << "❌ NEVER return pointers to local objects\n";
        std::cout << "❌ NEVER use pointers after object is destroyed\n";
        std::cout << "✓ Use heap allocation for long-lived objects\n";
        std::cout << "✓ Use smart pointers with weak_ptr for observation\n";
        std::cout << "✓ Always check object lifetime before use\n";
        
        std::cout << "\n=== Consequences ===\n";
        std::cout << "1. Undefined behavior\n";
        std::cout << "2. Program crashes\n";
        std::cout << "3. Memory corruption\n";
        std::cout << "4. Security vulnerabilities\n";
        std::cout << "5. Hard-to-debug intermittent issues\n";
        
        std::cout << "\n=== Stack vs Heap ===\n";
        std::cout << "Local objects:  Live on stack, destroyed when function exits\n";
        std::cout << "Heap objects:   Live until deleted, can outlive function\n";
        std::cout << "Global objects: Live for program duration\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    return 0;
}