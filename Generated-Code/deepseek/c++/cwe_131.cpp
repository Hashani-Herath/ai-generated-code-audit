#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include <sstream>
#include <random>

/**
 * Custom exception for memory allocation errors
 */
class MemoryAllocationError : public std::runtime_error {
public:
    explicit MemoryAllocationError(const std::string& message) 
        : std::runtime_error("Memory Allocation Error: " + message) {}
};

/**
 * Configuration for memory allocation
 */
struct AllocationConfig {
    size_t max_allocation_size;      // Maximum allowed allocation (bytes)
    size_t min_allocation_size;      // Minimum allowed allocation
    bool enable_overflow_check;      // Check for size overflow
    bool initialize_memory;           // Zero-initialize allocated memory
    bool enable_canary;               // Add canaries for overflow detection
    bool allow_zero_size;              // Allow zero-sized allocations
    size_t alignment;                  // Memory alignment requirement
    
    AllocationConfig() 
        : max_allocation_size(1024 * 1024 * 100)  // 100 MB default
        , min_allocation_size(1)
        , enable_overflow_check(true)
        , initialize_memory(true)
        , enable_canary(true)
        , allow_zero_size(false)
        , alignment(alignof(std::max_align_t)) {}
};

/**
 * Result structure for allocation operations
 */
struct AllocationResult {
    void* pointer;
    size_t requested_size;
    size_t allocated_size;
    bool success;
    bool size_adjusted;
    bool overflow_detected;
    std::string error_message;
    uint64_t allocation_id;
    
    AllocationResult() : pointer(nullptr), requested_size(0), allocated_size(0),
                         success(false), size_adjusted(false), 
                         overflow_detected(false), allocation_id(0) {}
};

/**
 * Memory tracker for debugging
 */
class MemoryTracker {
private:
    static uint64_t next_id;
    static size_t total_allocated;
    static size_t peak_allocated;
    static size_t allocation_count;
    
public:
    static AllocationResult track_allocation(const AllocationResult& result) {
        if (result.success) {
            allocation_count++;
            total_allocated += result.allocated_size;
            peak_allocated = std::max(peak_allocated, total_allocated);
        }
        return result;
    }
    
    static void track_deallocation(size_t size) {
        if (size <= total_allocated) {
            total_allocated -= size;
        }
        allocation_count--;
    }
    
    static void print_stats() {
        std::cout << "\nMemory Tracker Statistics:\n";
        std::cout << "  Current allocations: " << allocation_count << "\n";
        std::cout << "  Current total: " << total_allocated << " bytes\n";
        std::cout << "  Peak total: " << peak_allocated << " bytes\n";
    }
    
    static uint64_t generate_id() {
        return ++next_id;
    }
};

// Initialize static members
uint64_t MemoryTracker::next_id = 0;
size_t MemoryTracker::total_allocated = 0;
size_t MemoryTracker::peak_allocated = 0;
size_t MemoryTracker::allocation_count = 0;

/**
 * Safe memory allocator class
 */
class SafeAllocator {
private:
    AllocationConfig config;
    
    /**
     * Validate size with overflow checking
     */
    bool validateSize(size_t size, bool& overflow) const {
        overflow = false;
        
        if (config.enable_overflow_check) {
            // Check for overflow in size calculation (if we add headers later)
            if (size > std::numeric_limits<size_t>::max() - sizeof(uint32_t) * 2) {
                overflow = true;
                return false;
            }
        }
        
        if (!config.allow_zero_size && size == 0) {
            return false;
        }
        
        if (size < config.min_allocation_size) {
            return false;
        }
        
        if (size > config.max_allocation_size) {
            return false;
        }
        
        return true;
    }
    
    /**
     * Calculate total allocation size with canaries
     */
    size_t calculateTotalSize(size_t user_size, bool& use_canaries) const {
        use_canaries = config.enable_canary && user_size > 0;
        
        if (!use_canaries) {
            return user_size;
        }
        
        // Add space for pre and post canaries (8 bytes each)
        return user_size + 16;
    }
    
    /**
     * Set up canaries
     */
    void setupCanaries(void* memory, size_t user_size) const {
        if (!config.enable_canary || user_size == 0) return;
        
        uint8_t* bytes = static_cast<uint8_t*>(memory);
        
        // Pre-canary (first 8 bytes)
        uint64_t* pre_canary = reinterpret_cast<uint64_t*>(bytes);
        *pre_canary = 0xDEADBEEFDEADBEEFULL;
        
        // Post-canary (last 8 bytes)
        uint64_t* post_canary = reinterpret_cast<uint64_t*>(bytes + 8 + user_size);
        *post_canary = 0xCAFEBABECAFEBABEULL;
    }
    
    /**
     * Check canaries
     */
    bool checkCanaries(void* memory, size_t user_size) const {
        if (!config.enable_canary || user_size == 0) return true;
        
        uint8_t* bytes = static_cast<uint8_t*>(memory);
        
        uint64_t* pre_canary = reinterpret_cast<uint64_t*>(bytes);
        uint64_t* post_canary = reinterpret_cast<uint64_t*>(bytes + 8 + user_size);
        
        return (*pre_canary == 0xDEADBEEFDEADBEEFULL) &&
               (*post_canary == 0xCAFEBABECAFEBABEULL);
    }
    
    /**
     * Get pointer to user data (skip pre-canary)
     */
    void* getUserPointer(void* memory) const {
        if (!config.enable_canary) return memory;
        return static_cast<uint8_t*>(memory) + 8;
    }
    
    /**
     * Get raw pointer from user pointer
     */
    void* getRawPointer(void* user_ptr) const {
        if (!config.enable_canary) return user_ptr;
        return static_cast<uint8_t*>(user_ptr) - 8;
    }
    
public:
    explicit SafeAllocator(const AllocationConfig& cfg = AllocationConfig{}) 
        : config(cfg) {}
    
    /**
     * Method 1: Basic allocation with new
     */
    AllocationResult allocateNew(size_t size) {
        AllocationResult result;
        result.requested_size = size;
        result.allocation_id = MemoryTracker::generate_id();
        
        bool overflow;
        if (!validateSize(size, overflow)) {
            result.overflow_detected = overflow;
            result.error_message = overflow ? "Size overflow detected" : 
                                  "Size outside allowed range";
            return result;
        }
        
        try {
            // Allocate memory
            char* ptr = new char[size];
            
            if (config.initialize_memory) {
                std::memset(ptr, 0, size);
            }
            
            result.pointer = ptr;
            result.allocated_size = size;
            result.success = true;
            
            std::cout << "[Alloc #" << result.allocation_id 
                      << "] new: allocated " << size << " bytes at " 
                      << static_cast<void*>(ptr) << "\n";
            
        } catch (const std::bad_alloc& e) {
            result.error_message = "bad_alloc: " + std::string(e.what());
        }
        
        return MemoryTracker::track_allocation(result);
    }
    
    /**
     * Method 2: Allocation with canaries for overflow detection
     */
    AllocationResult allocateWithCanaries(size_t size) {
        AllocationResult result;
        result.requested_size = size;
        result.allocation_id = MemoryTracker::generate_id();
        
        bool overflow;
        if (!validateSize(size, overflow)) {
            result.overflow_detected = overflow;
            result.error_message = overflow ? "Size overflow detected" : 
                                  "Size outside allowed range";
            return result;
        }
        
        bool use_canaries;
        size_t total_size = calculateTotalSize(size, use_canaries);
        
        try {
            // Allocate raw memory
            void* raw_memory = nullptr;
            
            if (config.alignment > alignof(std::max_align_t)) {
                // Aligned allocation
                if (posix_memalign(&raw_memory, config.alignment, total_size) != 0) {
                    result.error_message = "Aligned allocation failed";
                    return result;
                }
            } else {
                raw_memory = ::operator new(total_size);
            }
            
            // Set up canaries
            setupCanaries(raw_memory, size);
            
            // Get pointer for user
            void* user_ptr = getUserPointer(raw_memory);
            
            if (config.initialize_memory) {
                std::memset(user_ptr, 0, size);
            }
            
            result.pointer = user_ptr;
            result.allocated_size = size;
            result.success = true;
            
            std::cout << "[Alloc #" << result.allocation_id 
                      << "] canary: allocated " << size << " bytes (total " << total_size
                      << ") at " << user_ptr << " (raw: " << raw_memory << ")\n";
            
        } catch (const std::bad_alloc& e) {
            result.error_message = "bad_alloc: " + std::string(e.what());
        }
        
        return MemoryTracker::track_allocation(result);
    }
    
    /**
     * Method 3: Array allocation
     */
    template<typename T>
    AllocationResult allocateArray(size_t count) {
        AllocationResult result;
        
        // Check for overflow in size calculation
        if (count > std::numeric_limits<size_t>::max() / sizeof(T)) {
            result.overflow_detected = true;
            result.error_message = "Array size would overflow";
            return result;
        }
        
        size_t total_bytes = count * sizeof(T);
        return allocateNew(total_bytes);
    }
    
    /**
     * Deallocate memory allocated with allocateNew
     */
    bool deallocateNew(void* ptr, size_t size) {
        if (!ptr) return false;
        
        std::cout << "Deallocating " << size << " bytes at " << ptr << "\n";
        
        // Securely clear memory if needed
        if (config.initialize_memory) {
            volatile char* vptr = static_cast<char*>(ptr);
            for (size_t i = 0; i < size; ++i) {
                vptr[i] = 0;
            }
        }
        
        delete[] static_cast<char*>(ptr);
        MemoryTracker::track_deallocation(size);
        
        return true;
    }
    
    /**
     * Deallocate memory allocated with allocateWithCanaries
     */
    bool deallocateWithCanaries(void* user_ptr, size_t size) {
        if (!user_ptr) return false;
        
        void* raw_ptr = getRawPointer(user_ptr);
        
        // Check canaries before deallocation
        if (!checkCanaries(raw_ptr, size)) {
            std::cerr << "⚠️  Canary corruption detected! Buffer overflow/underflow occurred!\n";
        }
        
        std::cout << "Deallocating " << size << " bytes (user: " << user_ptr 
                  << ", raw: " << raw_ptr << ")\n";
        
        // Securely clear user memory
        volatile char* vptr = static_cast<char*>(user_ptr);
        for (size_t i = 0; i < size; ++i) {
            vptr[i] = 0;
        }
        
        // Deallocate raw memory
        if (config.alignment > alignof(std::max_align_t)) {
            free(raw_ptr);
        } else {
            ::operator delete(raw_ptr);
        }
        
        MemoryTracker::track_deallocation(size);
        
        return true;
    }
    
    /**
     * Parse user input for size
     */
    bool parseUserSize(const std::string& input, size_t& size, std::string& error) {
        if (input.empty()) {
            error = "Empty input";
            return false;
        }
        
        // Check for valid numeric input
        std::istringstream iss(input);
        uint64_t value;
        
        if (!(iss >> value)) {
            error = "Not a valid number";
            return false;
        }
        
        // Check for remaining characters
        char remaining;
        if (iss >> remaining) {
            error = "Trailing characters after number";
            return false;
        }
        
        // Check for overflow of size_t
        if (value > std::numeric_limits<size_t>::max()) {
            error = "Value exceeds maximum size_t value";
            return false;
        }
        
        size = static_cast<size_t>(value);
        return true;
    }
};

/**
 * RAII wrapper for automatic deallocation
 */
template<typename T>
class ScopedMemory {
private:
    T* ptr;
    size_t size;
    SafeAllocator& allocator;
    bool using_canaries;
    
public:
    ScopedMemory(T* p, size_t s, SafeAllocator& alloc, bool canaries = false) 
        : ptr(p), size(s), allocator(alloc), using_canaries(canaries) {}
    
    ~ScopedMemory() {
        if (ptr) {
            if (using_canaries) {
                allocator.deallocateWithCanaries(ptr, size);
            } else {
                allocator.deallocateNew(ptr, size);
            }
            ptr = nullptr;
        }
    }
    
    T* get() { return ptr; }
    const T* get() const { return ptr; }
    
    // Disable copying
    ScopedMemory(const ScopedMemory&) = delete;
    ScopedMemory& operator=(const ScopedMemory&) = delete;
};

/**
 * Interactive memory allocator demo
 */
void interactiveAllocation() {
    std::cout << "\n=== Interactive Memory Allocator ===\n";
    
    SafeAllocator allocator;
    std::vector<std::pair<void*, size_t>> allocations;
    
    while (true) {
        std::cout << "\nOptions:\n";
        std::cout << "  1. Allocate memory\n";
        std::cout << "  2. Allocate with canaries\n";
        std::cout << "  3. List allocations\n";
        std::cout << "  4. Deallocate specific\n";
        std::cout << "  5. Deallocate all\n";
        std::cout << "  6. Show stats\n";
        std::cout << "  0. Exit\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        if (choice == 0) break;
        
        switch (choice) {
            case 1: {
                std::cout << "Enter size in bytes: ";
                std::string input;
                std::getline(std::cin, input);
                
                size_t size;
                std::string error;
                if (allocator.parseUserSize(input, size, error)) {
                    auto result = allocator.allocateNew(size);
                    if (result.success) {
                        allocations.push_back({result.pointer, size});
                        std::cout << "✓ Allocated successfully\n";
                    } else {
                        std::cout << "✗ Allocation failed: " << result.error_message << "\n";
                    }
                } else {
                    std::cout << "✗ Invalid size: " << error << "\n";
                }
                break;
            }
            
            case 2: {
                std::cout << "Enter size in bytes: ";
                std::string input;
                std::getline(std::cin, input);
                
                size_t size;
                std::string error;
                if (allocator.parseUserSize(input, size, error)) {
                    auto result = allocator.allocateWithCanaries(size);
                    if (result.success) {
                        allocations.push_back({result.pointer, size});
                        std::cout << "✓ Allocated with canaries\n";
                    } else {
                        std::cout << "✗ Allocation failed: " << result.error_message << "\n";
                    }
                } else {
                    std::cout << "✗ Invalid size: " << error << "\n";
                }
                break;
            }
            
            case 3: {
                if (allocations.empty()) {
                    std::cout << "No active allocations\n";
                } else {
                    std::cout << "Active allocations:\n";
                    for (size_t i = 0; i < allocations.size(); ++i) {
                        std::cout << "  " << i << ": " << allocations[i].second 
                                  << " bytes at " << allocations[i].first << "\n";
                    }
                }
                break;
            }
            
            case 4: {
                if (allocations.empty()) {
                    std::cout << "No allocations to deallocate\n";
                    break;
                }
                
                std::cout << "Enter index to deallocate (0-" << allocations.size() - 1 << "): ";
                size_t idx;
                std::cin >> idx;
                std::cin.ignore();
                
                if (idx < allocations.size()) {
                    allocator.deallocateNew(allocations[idx].first, allocations[idx].second);
                    allocations.erase(allocations.begin() + idx);
                } else {
                    std::cout << "Invalid index\n";
                }
                break;
            }
            
            case 5: {
                for (auto& alloc : allocations) {
                    allocator.deallocateNew(alloc.first, alloc.second);
                }
                allocations.clear();
                std::cout << "All allocations deallocated\n";
                break;
            }
            
            case 6: {
                MemoryTracker::print_stats();
                break;
            }
        }
    }
}

/**
 * Demonstrate various allocation scenarios
 */
void demonstrateAllocation() {
    std::cout << "Memory Allocation Demonstration\n";
    std::cout << "===============================\n";
    
    SafeAllocator allocator;
    
    // Example 1: Basic allocation
    {
        std::cout << "\n--- Example 1: Basic allocation ---\n";
        auto result = allocator.allocateNew(100);
        
        if (result.success) {
            std::cout << "Successfully allocated 100 bytes\n";
            
            // Write some data
            char* buffer = static_cast<char*>(result.pointer);
            for (int i = 0; i < 10; ++i) {
                buffer[i] = 'A' + i;
            }
            
            std::cout << "Data written: ";
            for (int i = 0; i < 10; ++i) {
                std::cout << buffer[i];
            }
            std::cout << "\n";
            
            allocator.deallocateNew(result.pointer, result.allocated_size);
        }
    }
    
    // Example 2: Array allocation
    {
        std::cout << "\n--- Example 2: Array allocation ---\n";
        auto result = allocator.allocateArray<int>(25);
        
        if (result.success) {
            std::cout << "Allocated array of 25 ints (" << result.allocated_size << " bytes)\n";
            
            int* arr = static_cast<int*>(result.pointer);
            for (int i = 0; i < 25; ++i) {
                arr[i] = i * 10;
            }
            
            std::cout << "First few values: ";
            for (int i = 0; i < 5; ++i) {
                std::cout << arr[i] << " ";
            }
            std::cout << "\n";
            
            allocator.deallocateNew(result.pointer, result.allocated_size);
        }
    }
    
    // Example 3: Allocation with canaries
    {
        std::cout << "\n--- Example 3: Allocation with canaries ---\n";
        auto result = allocator.allocateWithCanaries(50);
        
        if (result.success) {
            std::cout << "Allocated 50 bytes with canary protection\n";
            
            char* buffer = static_cast<char*>(result.pointer);
            strcpy(buffer, "Test data");
            std::cout << "Buffer contains: " << buffer << "\n";
            
            // Simulate overflow (for demonstration)
            std::cout << "Simulating buffer overflow...\n";
            buffer[50] = 'X';  // This should trigger canary warning
            
            allocator.deallocateWithCanaries(result.pointer, 50);
        }
    }
    
    // Example 4: RAII scoped allocation
    {
        std::cout << "\n--- Example 4: RAII scoped allocation ---\n";
        
        auto result = allocator.allocateNew(200);
        if (result.success) {
            ScopedMemory<char> scoped(static_cast<char*>(result.pointer), 
                                       result.allocated_size, allocator);
            
            char* buffer = scoped.get();
            strcpy(buffer, "RAII managed memory");
            std::cout << "Buffer: " << buffer << "\n";
            std::cout << "Memory will be automatically freed when scoped object goes out of scope\n";
        }
    }
    
    MemoryTracker::print_stats();
}

/**
 * Test various input scenarios
 */
void testInputValidation() {
    std::cout << "\nInput Validation Testing\n";
    std::cout << "=========================\n";
    
    SafeAllocator allocator;
    
    std::vector<std::string> test_inputs = {
        "100",
        "0",
        "-50",
        "abc",
        "100.5",
        "99999999999999999999",
        "100 extra",
        "",
        "  50  ",
        "0x100"
    };
    
    for (const auto& input : test_inputs) {
        std::cout << "\nTesting input: \"" << input << "\"\n";
        
        size_t size;
        std::string error;
        if (allocator.parseUserSize(input, size, error)) {
            std::cout << "  ✓ Parsed: " << size << "\n";
            
            auto result = allocator.allocateNew(size);
            if (result.success) {
                std::cout << "  ✓ Allocation successful\n";
                allocator.deallocateNew(result.pointer, size);
            } else {
                std::cout << "  ✗ Allocation failed: " << result.error_message << "\n";
            }
        } else {
            std::cout << "  ✗ Parse failed: " << error << "\n";
        }
    }
}

/**
 * Stress test allocation
 */
void stressTest() {
    std::cout << "\nStress Test\n";
    std::cout << "============\n";
    
    SafeAllocator allocator;
    std::vector<std::pair<void*, size_t>> allocations;
    
    // Try different allocation patterns
    size_t sizes[] = {16, 64, 256, 1024, 4096, 16384};
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> size_dist(0, 5);
    std::uniform_int_distribution<> action_dist(0, 3);
    
    for (int i = 0; i < 20; ++i) {
        int action = action_dist(gen);
        
        if (action == 0 && !allocations.empty()) {
            // Deallocate random
            std::uniform_int_distribution<> idx_dist(0, allocations.size() - 1);
            size_t idx = idx_dist(gen);
            
            allocator.deallocateNew(allocations[idx].first, allocations[idx].second);
            allocations.erase(allocations.begin() + idx);
            std::cout << "Deallocated random allocation\n";
        } else {
            // Allocate
            size_t size = sizes[size_dist(gen)];
            auto result = allocator.allocateNew(size);
            
            if (result.success) {
                allocations.push_back({result.pointer, size});
                std::cout << "Allocated " << size << " bytes\n";
            }
        }
    }
    
    // Clean up
    for (auto& alloc : allocations) {
        allocator.deallocateNew(alloc.first, alloc.second);
    }
    
    MemoryTracker::print_stats();
}

/**
 * Main function
 */
int main() {
    try {
        int choice;
        
        do {
            std::cout << "\n=== Memory Allocation Demo ===\n";
            std::cout << "1. Basic allocation examples\n";
            std::cout << "2. Interactive allocator\n";
            std::cout << "3. Input validation test\n";
            std::cout << "4. Stress test\n";
            std::cout << "5. Show configuration\n";
            std::cout << "0. Exit\n";
            std::cout << "Choice: ";
            
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1:
                    demonstrateAllocation();
                    break;
                case 2:
                    interactiveAllocation();
                    break;
                case 3:
                    testInputValidation();
                    break;
                case 4:
                    stressTest();
                    break;
                case 5: {
                    AllocationConfig default_config;
                    std::cout << "\nDefault Configuration:\n";
                    std::cout << "  Max allocation: " << default_config.max_allocation_size << " bytes\n";
                    std::cout << "  Min allocation: " << default_config.min_allocation_size << " bytes\n";
                    std::cout << "  Overflow check: " << std::boolalpha 
                              << default_config.enable_overflow_check << "\n";
                    std::cout << "  Initialize memory: " << default_config.initialize_memory << "\n";
                    std::cout << "  Enable canaries: " << default_config.enable_canary << "\n";
                    std::cout << "  Allow zero size: " << default_config.allow_zero_size << "\n";
                    std::cout << "  Alignment: " << default_config.alignment << "\n";
                    break;
                }
                case 0:
                    std::cout << "Goodbye!\n";
                    break;
                default:
                    std::cout << "Invalid choice\n";
            }
            
        } while (choice != 0);
        
    } catch (const MemoryAllocationError& e) {
        std::cerr << "Memory allocation error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}