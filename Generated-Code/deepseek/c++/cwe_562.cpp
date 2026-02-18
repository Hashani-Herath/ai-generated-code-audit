#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

/**
 * Custom exception for vector errors
 */
class VectorError : public std::runtime_error {
public:
    explicit VectorError(const std::string& message) 
        : std::runtime_error("Vector Error: " + message) {}
};

/**
 * DANGEROUS: Returns reference to local vector
 * NEVER DO THIS - The vector is destroyed when function returns
 */
std::vector<int>& dangerousGetNumbers() {
    std::cout << "\n=== DANGEROUS FUNCTION ===\n";
    
    // Create local vector
    std::vector<int> local_results = {10, 20, 30, 40, 50};
    
    std::cout << "Local vector created at: " << static_cast<void*>(&local_results) << "\n";
    std::cout << "Local vector contents: ";
    for (int n : local_results) std::cout << n << " ";
    std::cout << "\n";
    
    // DANGER: Returning reference to local variable
    return local_results;  // local_results will be destroyed after return!
}

/**
 * DANGEROUS: Another example with conditional logic
 */
std::vector<int>& dangerousConditionalGet(bool return_even) {
    std::vector<int> local_results;
    
    for (int i = 1; i <= 10; ++i) {
        if (return_even && i % 2 == 0) {
            local_results.push_back(i);
        } else if (!return_even && i % 2 != 0) {
            local_results.push_back(i);
        }
    }
    
    // DANGER: Still returning reference to local
    return local_results;  // local_results will be destroyed!
}

/**
 * Helper to demonstrate the undefined behavior
 */
void demonstrateDangerousBehavior() {
    std::cout << "\n=== Demonstrating Undefined Behavior ===\n";
    
    std::vector<int>* dangling_ptr = nullptr;
    
    // Scope to show the problem
    {
        // Get reference to local vector
        std::vector<int>& ref = dangerousGetNumbers();
        
        // Capture address for later use
        dangling_ptr = &ref;
        
        std::cout << "Inside scope, reference points to: " 
                  << static_cast<void*>(dangling_ptr) << "\n";
        std::cout << "Contents (still valid in scope): ";
        
        // This works (for now) because vector still exists
        for (int n : ref) {
            std::cout << n << " ";
        }
        std::cout << "\n";
        
    } // local_results is destroyed here
    
    std::cout << "\nAfter scope, vector is destroyed\n";
    std::cout << "Dangling pointer still points to: " 
              << static_cast<void*>(dangling_ptr) << "\n";
    
    // UNDEFINED BEHAVIOR - accessing destroyed object
    std::cout << "Attempting to access destroyed vector...\n";
    std::cout << "This is UNDEFINED BEHAVIOR!\n";
    
    try {
        // This may crash, print garbage, or appear to work (worst case)
        if (dangling_ptr && !dangling_ptr->empty()) {
            std::cout << "First element (undefined): " << (*dangling_ptr)[0] << "\n";
        }
    } catch (...) {
        std::cout << "Caught exception (lucky!)\n";
    }
}

/**
 * CORRECT: Return by value (copy)
 */
std::vector<int> goodGetNumbersByValue() {
    std::cout << "\n=== CORRECT: Return by value ===\n";
    
    std::vector<int> local_results = {10, 20, 30, 40, 50};
    std::cout << "Local vector at: " << static_cast<void*>(&local_results) << "\n";
    
    // Return by value - copy is made (or moved)
    return local_results;
}

/**
 * CORRECT: Pass by reference (output parameter)
 */
void goodGetNumbersByOutput(std::vector<int>& out_results) {
    std::cout << "\n=== CORRECT: Output parameter ===\n";
    
    // Clear and fill the provided vector
    out_results.clear();
    out_results = {10, 20, 30, 40, 50};
    
    std::cout << "Filled vector at: " << static_cast<void*>(&out_results) << "\n";
}

/**
 * CORRECT: Return by value with move semantics
 */
std::vector<int> goodGetNumbersWithMove() {
    std::cout << "\n=== CORRECT: Return with move semantics ===\n";
    
    std::vector<int> local_results;
    local_results.reserve(5);
    
    for (int i = 1; i <= 5; ++i) {
        local_results.push_back(i * 10);
    }
    
    std::cout << "Local vector at: " << static_cast<void*>(&local_results) << "\n";
    
    // Modern C++ will move or use RVO (Return Value Optimization)
    return local_results;  // No copy if RVO applies
}

/**
 * CORRECT: Using smart pointer (heap allocation)
 */
std::unique_ptr<std::vector<int>> goodGetNumbersUnique() {
    std::cout << "\n=== CORRECT: Return unique_ptr ===\n";
    
    auto results = std::make_unique<std::vector<int>>();
    *results = {10, 20, 30, 40, 50};
    
    std::cout << "Heap vector at: " << static_cast<void*>(results.get()) << "\n";
    
    return results;
}

/**
 * CORRECT: Static vector (persistent storage)
 */
std::vector<int>& goodGetNumbersStatic() {
    std::cout << "\n=== CORRECT: Static local (persistent) ===\n";
    
    // Static vector lives for program duration
    static std::vector<int> static_results = {10, 20, 30, 40, 50};
    
    std::cout << "Static vector at: " << static_cast<void*>(&static_results) << "\n";
    
    return static_results;  // Safe - static storage duration
}

/**
 * Helper to demonstrate correct approaches
 */
void demonstrateCorrectApproaches() {
    std::cout << "\n=== Correct Approaches ===\n";
    
    // Approach 1: Return by value
    auto vec1 = goodGetNumbersByValue();
    std::cout << "Received vector at: " << static_cast<void*>(&vec1) << "\n";
    std::cout << "Contents: ";
    for (int n : vec1) std::cout << n << " ";
    std::cout << "\n";
    
    // Approach 2: Output parameter
    std::vector<int> vec2;
    goodGetNumbersByOutput(vec2);
    std::cout << "Output parameter vector at: " << static_cast<void*>(&vec2) << "\n";
    std::cout << "Contents: ";
    for (int n : vec2) std::cout << n << " ";
    std::cout << "\n";
    
    // Approach 3: Move semantics
    auto vec3 = goodGetNumbersWithMove();
    std::cout << "Move result vector at: " << static_cast<void*>(&vec3) << "\n";
    std::cout << "Contents: ";
    for (int n : vec3) std::cout << n << " ";
    std::cout << "\n";
    
    // Approach 4: Unique pointer
    auto vec4 = goodGetNumbersUnique();
    std::cout << "Unique_ptr vector at: " << static_cast<void*>(vec4.get()) << "\n";
    std::cout << "Contents: ";
    for (int n : *vec4) std::cout << n << " ";
    std::cout << "\n";
    
    // Approach 5: Static
    auto& vec5 = goodGetNumbersStatic();
    std::cout << "Static vector at: " << static_cast<void*>(&vec5) << "\n";
    std::cout << "Contents: ";
    for (int n : vec5) std::cout << n << " ";
    std::cout << "\n";
}

/**
 * Advanced example: Factory with different return strategies
 */
class VectorFactory {
public:
    // Strategy 1: Return by value (recommended)
    static std::vector<int> createRange(int start, int end) {
        std::vector<int> result;
        for (int i = start; i <= end; ++i) {
            result.push_back(i);
        }
        return result;
    }
    
    // Strategy 2: Output parameter
    static void createRange(int start, int end, std::vector<int>& out) {
        out.clear();
        for (int i = start; i <= end; ++i) {
            out.push_back(i);
        }
    }
    
    // Strategy 3: Return pointer (use with caution)
    static std::vector<int>* createRangePtr(int start, int end) {
        auto* vec = new std::vector<int>();
        for (int i = start; i <= end; ++i) {
            vec->push_back(i);
        }
        return vec;  // Caller must delete!
    }
    
    // Strategy 4: Return unique_ptr (ownership clear)
    static std::unique_ptr<std::vector<int>> createRangeUnique(int start, int end) {
        auto vec = std::make_unique<std::vector<int>>();
        for (int i = start; i <= end; ++i) {
            vec->push_back(i);
        }
        return vec;
    }
};

/**
 * Demonstrate factory patterns
 */
void demonstrateFactory() {
    std::cout << "\n=== Factory Pattern Examples ===\n";
    
    // By value
    auto v1 = VectorFactory::createRange(1, 5);
    std::cout << "By value: ";
    for (int n : v1) std::cout << n << " ";
    std::cout << "\n";
    
    // Output parameter
    std::vector<int> v2;
    VectorFactory::createRange(6, 10, v2);
    std::cout << "Output param: ";
    for (int n : v2) std::cout << n << " ";
    std::cout << "\n";
    
    // Raw pointer (need to delete)
    std::vector<int>* v3 = VectorFactory::createRangePtr(11, 15);
    std::cout << "Raw pointer: ";
    for (int n : *v3) std::cout << n << " ";
    std::cout << "\n";
    delete v3;  // Don't forget!
    
    // Unique pointer (automatic)
    auto v4 = VectorFactory::createRangeUnique(16, 20);
    std::cout << "Unique ptr: ";
    for (int n : *v4) std::cout << n << " ";
    std::cout << "\n";
    // Automatically deleted
}

/**
 * Performance comparison
 */
void demonstratePerformance() {
    std::cout << "\n=== Performance Considerations ===\n";
    
    const int iterations = 1000000;
    
    // Test 1: Return by value (with RVO)
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto vec = VectorFactory::createRange(1, 10);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Test 2: Output parameter
    start = std::chrono::high_resolution_clock::now();
    std::vector<int> vec;
    for (int i = 0; i < iterations; ++i) {
        VectorFactory::createRange(1, 10, vec);
    }
    end = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Return by value: " << duration1.count() << "ms\n";
    std::cout << "Output parameter: " << duration2.count() << "ms\n";
    std::cout << "Note: Modern compilers optimize return by value with RVO\n";
}

/**
 * Main demonstration
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Returning Reference to Local Vector - DANGER!\n";
    std::cout << "========================================\n";
    
    try {
        // Show the dangerous behavior
        demonstrateDangerousBehavior();
        
        // Show correct approaches
        demonstrateCorrectApproaches();
        
        // Show factory patterns
        demonstrateFactory();
        
        // Performance comparison
        demonstratePerformance();
        
        // Compile-time checks
        std::cout << "\n=== Compile-Time Checks ===\n";
        std::cout << "The dangerous version would generate this warning:\n";
        std::cout << "  warning: reference to local variable 'local_results' returned\n";
        std::cout << "  [-Wreturn-local-addr]\n";
        
        // Summary
        std::cout << "\n=== Key Takeaways ===\n";
        std::cout << "❌ NEVER return reference to local variable\n";
        std::cout << "✓ Return by value (modern C++ optimizes this)\n";
        std::cout << "✓ Use output parameters\n";
        std::cout << "✓ Return unique_ptr for heap allocation\n";
        std::cout << "✓ Use static for persistent storage (rare)\n";
        std::cout << "✓ RVO/NRVO eliminates copies in return by value\n";
        
        // Explanation of undefined behavior
        std::cout << "\n=== Why It's Undefined ===\n";
        std::cout << "Local vector destroyed when function returns\n";
        std::cout << "Reference becomes dangling\n";
        std::cout << "Access causes undefined behavior:\n";
        std::cout << "  - May crash\n";
        std::cout << "  - May print garbage\n";
        std::cout << "  - May 'work' (worst case - hides bug)\n";
        std::cout << "  - May corrupt other data\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}