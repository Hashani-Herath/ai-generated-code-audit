#include <iostream>
#include <iomanip>
#include <cstring>
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <type_traits>

/**
 * Custom exception for heap allocation errors
 */
class HeapAllocationError : public std::runtime_error {
public:
    explicit HeapAllocationError(const std::string& message) 
        : std::runtime_error("Heap Allocation Error: " + message) {}
};

/**
 * Configuration for heap buffer allocation
 */
struct HeapBufferConfig {
    size_t default_size;
    bool initialize_to_zero;
    bool check_overflow;
    bool enable_canary;
    size_t max_allocation_size;
    size_t alignment;
    
    HeapBufferConfig() 
        : default_size(1024)
        , initialize_to_zero(true)
        , check_overflow(true)
        , enable_canary(true)
        , max_allocation_size(1024 * 1024 * 100)  // 100 MB
        , alignment(alignof(std::max_align_t)) {}
};

/**
 * Result structure for allocation operations
 */
struct AllocationResult {
    void* pointer;
    size_t size;
    bool success;
    std::string error_message;
    size_t allocation_id;
    
    AllocationResult() : pointer(nullptr), size(0), success(false), allocation_id(0) {}
};

/**
 * Simple canary value for overflow detection
 */
class Canary {
private:
    static constexpr uint32_t CANARY_VALUE = 0xDEADBEEF;
    uint32_t value;
    
public:
    Canary() : value(CANARY_VALUE) {}
    
    bool isIntact() const {
        return value == CANARY_VALUE;
    }
    
    void corrupt() {
        value = 0;
    }
};

/**
 * Heap buffer class with RAII and safety features
 */
template<typename T>
class HeapBuffer {
private:
    T* data;
    size_t count;
    size_t allocation_id;
    static size_t next_allocation_id;
    
    // Optional canaries for overflow detection
    Canary* pre_canary;
    Canary* post_canary;
    bool canaries_enabled;
    
    /**
     * Validate size before allocation
     */
    void validateSize(size_t size) const {
        if (size == 0) {
            throw HeapAllocationError("Cannot allocate zero-sized buffer");
        }
        
        if (size > config.max_allocation_size) {
            throw HeapAllocationError("Allocation size exceeds maximum limit");
        }
        
        // Check for overflow in size calculation
        if (size > std::numeric_limits<size_t>::max() / sizeof(T)) {
            throw HeapAllocationError("Size calculation would overflow");
        }
    }
    
    /**
     * Calculate total allocation size including canaries
     */
    size_t calculateTotalSize(size_t element_count) const {
        size_t data_size = element_count * sizeof(T);
        
        if (canaries_enabled) {
            // Add space for pre and post canaries
            return data_size + 2 * sizeof(Canary);
        }
        
        return data_size;
    }
    
    /**
     * Get pointer to data area (accounting for pre-canary)
     */
    T* getDataPointer(void* raw_memory) const {
        if (canaries_enabled) {
            return reinterpret_cast<T*>(
                static_cast<char*>(raw_memory) + sizeof(Canary)
            );
        }
        return static_cast<T*>(raw_memory);
    }
    
    /**
     * Get pointer to pre-canary
     */
    Canary* getPreCanary(void* raw_memory) const {
        if (canaries_enabled) {
            return static_cast<Canary*>(raw_memory);
        }
        return nullptr;
    }
    
    /**
     * Get pointer to post-canary
     */
    Canary* getPostCanary(void* raw_memory, size_t element_count) const {
        if (canaries_enabled) {
            return reinterpret_cast<Canary*>(
                static_cast<char*>(raw_memory) + sizeof(Canary) + 
                element_count * sizeof(T)
            );
        }
        return nullptr;
    }
    
public:
    static HeapBufferConfig config;
    
    /**
     * Constructor - allocates heap buffer
     */
    explicit HeapBuffer(size_t size = config.default_size) 
        : data(nullptr)
        , count(0)
        , allocation_id(++next_allocation_id)
        , pre_canary(nullptr)
        , post_canary(nullptr)
        , canaries_enabled(config.enable_canary) {
        
        std::cout << "[HeapBuffer #" << allocation_id << "] Creating buffer...\n";
        
        try {
            validateSize(size);
            
            size_t total_size = calculateTotalSize(size);
            
            // Allocate raw memory
            void* raw_memory = nullptr;
            
            if (config.alignment > alignof(std::max_align_t)) {
                // Aligned allocation
                if (posix_memalign(&raw_memory, config.alignment, total_size) != 0) {
                    throw HeapAllocationError("Aligned allocation failed");
                }
            } else {
                // Regular allocation
                raw_memory = ::operator new(total_size);
            }
            
            if (!raw_memory) {
                throw HeapAllocationError("Allocation failed");
            }
            
            // Set up canaries if enabled
            if (canaries_enabled) {
                pre_canary = getPreCanary(raw_memory);
                post_canary = getPostCanary(raw_memory, size);
                
                // Initialize canaries
                new (pre_canary) Canary();
                new (post_canary) Canary();
            }
            
            // Get pointer to data area
            data = getDataPointer(raw_memory);
            count = size;
            
            // Initialize memory if configured
            if (config.initialize_to_zero) {
                std::memset(data, 0, size * sizeof(T));
            }
            
            std::cout << "[HeapBuffer #" << allocation_id << "] Allocated " 
                      << size << " elements (" << size * sizeof(T) 
                      << " bytes) at " << static_cast<void*>(data) << "\n";
            
            if (canaries_enabled) {
                std::cout << "[HeapBuffer #" << allocation_id << "] Canaries: pre=0x" 
                          << std::hex << pre_canary << ", post=0x" << post_canary 
                          << std::dec << "\n";
            }
            
        } catch (const std::exception& e) {
            std::cerr << "[HeapBuffer #" << allocation_id 
                      << "] Allocation failed: " << e.what() << "\n";
            throw;
        }
    }
    
    /**
     * Destructor - safely deallocates buffer
     */
    ~HeapBuffer() {
        if (data) {
            std::cout << "[HeapBuffer #" << allocation_id << "] Destroying buffer at " 
                      << static_cast<void*>(data) << "\n";
            
            // Check canaries before deallocation
            if (canaries_enabled && config.check_overflow) {
                if (pre_canary && !pre_canary->isIntact()) {
                    std::cerr << "[HeapBuffer #" << allocation_id 
                              << "] ⚠️  PRE-CANARY CORRUPTED! Buffer underflow detected!\n";
                }
                if (post_canary && !post_canary->isIntact()) {
                    std::cerr << "[HeapBuffer #" << allocation_id 
                              << "] ⚠️  POST-CANARY CORRUPTED! Buffer overflow detected!\n";
                }
            }
            
            // Securely clear sensitive data if needed
            if (std::is_trivially_copyable<T>::value) {
                volatile T* vptr = data;
                for (size_t i = 0; i < count; ++i) {
                    vptr[i] = T();
                }
            }
            
            // Get raw memory pointer (accounting for pre-canary)
            void* raw_memory = canaries_enabled ? 
                static_cast<void*>(pre_canary) : static_cast<void*>(data);
            
            // Deallocate
            if (config.alignment > alignof(std::max_align_t)) {
                free(raw_memory);
            } else {
                ::operator delete(raw_memory);
            }
            
            data = nullptr;
            count = 0;
            pre_canary = nullptr;
            post_canary = nullptr;
            
            std::cout << "[HeapBuffer #" << allocation_id << "] Buffer deallocated\n";
        }
    }
    
    // Disable copying
    HeapBuffer(const HeapBuffer&) = delete;
    HeapBuffer& operator=(const HeapBuffer&) = delete;
    
    // Enable moving
    HeapBuffer(HeapBuffer&& other) noexcept
        : data(other.data)
        , count(other.count)
        , allocation_id(other.allocation_id)
        , pre_canary(other.pre_canary)
        , post_canary(other.post_canary)
        , canaries_enabled(other.canaries_enabled) {
        
        other.data = nullptr;
        other.count = 0;
        other.pre_canary = nullptr;
        other.post_canary = nullptr;
    }
    
    HeapBuffer& operator=(HeapBuffer&& other) noexcept {
        if (this != &other) {
            this->~HeapBuffer();
            
            data = other.data;
            count = other.count;
            allocation_id = other.allocation_id;
            pre_canary = other.pre_canary;
            post_canary = other.post_canary;
            canaries_enabled = other.canaries_enabled;
            
            other.data = nullptr;
            other.count = 0;
            other.pre_canary = nullptr;
            other.post_canary = nullptr;
        }
        return *this;
    }
    
    // Accessors
    T* get() { return data; }
    const T* get() const { return data; }
    size_t size() const { return count; }
    size_t sizeInBytes() const { return count * sizeof(T); }
    
    T& operator[](size_t index) {
        if (config.check_overflow && index >= count) {
            throw std::out_of_range("HeapBuffer index out of range");
        }
        return data[index];
    }
    
    const T& operator[](size_t index) const {
        if (config.check_overflow && index >= count) {
            throw std::out_of_range("HeapBuffer index out of range");
        }
        return data[index];
    }
    
    /**
     * Fill buffer with a pattern
     */
    void fill(const T& value) {
        std::fill(data, data + count, value);
    }
    
    /**
     * Check buffer integrity (canaries)
     */
    bool checkIntegrity() const {
        if (!canaries_enabled || !pre_canary || !post_canary) {
            return true;
        }
        
        return pre_canary->isIntact() && post_canary->isIntact();
    }
    
    /**
     * Get allocation ID
     */
    size_t getAllocationId() const {
        return allocation_id;
    }
};

// Initialize static members
template<typename T>
size_t HeapBuffer<T>::next_allocation_id = 0;

template<typename T>
HeapBufferConfig HeapBuffer<T>::config;

/**
 * Simple RAII wrapper for raw heap allocations
 */
class RawHeapBuffer {
private:
    char* data;
    size_t size;
    
public:
    RawHeapBuffer(size_t bytes) : size(bytes) {
        data = new char[bytes];
        std::cout << "RawHeapBuffer: Allocated " << bytes << " bytes\n";
        
        if (HeapBuffer<char>::config.initialize_to_zero) {
            std::memset(data, 0, bytes);
        }
    }
    
    ~RawHeapBuffer() {
        if (data) {
            // Secure clear before deletion
            volatile char* vptr = data;
            for (size_t i = 0; i < size; ++i) {
                vptr[i] = 0;
            }
            
            delete[] data;
            std::cout << "RawHeapBuffer: Deallocated " << size << " bytes\n";
        }
    }
    
    // Disable copy
    RawHeapBuffer(const RawHeapBuffer&) = delete;
    RawHeapBuffer& operator=(const RawHeapBuffer&) = delete;
    
    // Accessors
    char* get() { return data; }
    const char* get() const { return data; }
    size_t getSize() const { return size; }
};

/**
 * Demonstrate various heap allocation techniques
 */
void demonstrateHeapAllocation() {
    std::cout << "Heap Buffer Allocation Demonstration\n";
    std::cout << "====================================\n";
    
    // Example 1: Basic allocation
    {
        std::cout << "\n--- Example 1: Basic allocation ---\n";
        HeapBuffer<int> buffer1(10);
        buffer1.fill(42);
        
        for (size_t i = 0; i < buffer1.size(); ++i) {
            std::cout << "buffer1[" << i << "] = " << buffer1[i] << "\n";
        }
    } // Automatically deallocated
    
    // Example 2: Move semantics
    {
        std::cout << "\n--- Example 2: Move semantics ---\n";
        HeapBuffer<double> buffer2(5);
        buffer2.fill(3.14159);
        
        std::cout << "Original buffer address: " << static_cast<void*>(buffer2.get()) << "\n";
        
        HeapBuffer<double> buffer3 = std::move(buffer2);
        std::cout << "Moved buffer address: " << static_cast<void*>(buffer3.get()) << "\n";
        std::cout << "Original buffer now: " << static_cast<void*>(buffer2.get()) << "\n";
        
        for (size_t i = 0; i < buffer3.size(); ++i) {
            std::cout << "buffer3[" << i << "] = " << buffer3[i] << "\n";
        }
    }
    
    // Example 3: Bounds checking
    {
        std::cout << "\n--- Example 3: Bounds checking ---\n";
        HeapBuffer<int> buffer4(3);
        buffer4[0] = 100;
        buffer4[1] = 200;
        buffer4[2] = 300;
        
        try {
            std::cout << "Attempting to access index 5...\n";
            std::cout << buffer4[5] << "\n";  // Should throw
        } catch (const std::out_of_range& e) {
            std::cout << "Caught expected exception: " << e.what() << "\n";
        }
    }
    
    // Example 4: Different types
    {
        std::cout << "\n--- Example 4: Different data types ---\n";
        HeapBuffer<char> char_buffer(10);
        char_buffer.fill('A');
        
        HeapBuffer<double> double_buffer(5);
        double_buffer.fill(1.2345);
        
        std::cout << "char buffer: ";
        for (size_t i = 0; i < char_buffer.size(); ++i) {
            std::cout << char_buffer[i];
        }
        std::cout << "\n";
        
        std::cout << "double buffer: ";
        for (size_t i = 0; i < double_buffer.size(); ++i) {
            std::cout << double_buffer[i] << " ";
        }
        std::cout << "\n";
    }
}

/**
 * Demonstrate error handling
 */
void demonstrateErrorHandling() {
    std::cout << "\n\nError Handling Demonstration\n";
    std::cout << "============================\n";
    
    // Example 1: Zero size
    try {
        std::cout << "Attempting to allocate zero-size buffer...\n";
        HeapBuffer<int> bad_buffer(0);
    } catch (const HeapAllocationError& e) {
        std::cout << "Caught expected error: " << e.what() << "\n";
    }
    
    // Example 2: Excessive size
    try {
        std::cout << "Attempting to allocate excessive buffer...\n";
        HeapBuffer<int> huge_buffer(HeapBuffer<int>::config.max_allocation_size + 1);
    } catch (const HeapAllocationError& e) {
        std::cout << "Caught expected error: " << e.what() << "\n";
    }
}

/**
 * Demonstrate canary protection
 */
void demonstrateCanaryProtection() {
    std::cout << "\n\nCanary Protection Demonstration\n";
    std::cout << "===============================\n";
    
    // Enable canaries
    HeapBuffer<char>::config.enable_canary = true;
    HeapBuffer<char>::config.check_overflow = true;
    
    {
        HeapBuffer<char> protected_buffer(10);
        
        std::cout << "Buffer created with canaries\n";
        
        // Simulate buffer overflow (for demonstration only!)
        std::cout << "Simulating buffer overflow...\n";
        char* raw = protected_buffer.get();
        
        // This would normally be dangerous - we're doing it for demonstration
        // In real code, NEVER do this!
        raw[10] = 'X';  // Write just past the end
        raw[-1] = 'Y';  // Write just before the beginning
        
        std::cout << "Buffer integrity check: " 
                  << (protected_buffer.checkIntegrity() ? "OK" : "CORRUPTED") << "\n";
        
    } // Destructor will report canary corruption
}

/**
 * Demonstrate raw vs RAII allocation
 */
void demonstrateRawVsRAII() {
    std::cout << "\n\nRaw vs RAII Allocation\n";
    std::cout << "======================\n";
    
    // RAII approach (safe)
    {
        HeapBuffer<int> raii_buffer(100);
        std::cout << "RAII buffer automatically managed\n";
    } // Automatically freed
    
    // Raw approach (unsafe, but shown for comparison)
    {
        std::cout << "\nRaw allocation (manual management):\n";
        int* raw_buffer = new int[50];
        std::cout << "Raw buffer allocated - must remember to delete\n";
        
        // ... use buffer ...
        
        delete[] raw_buffer;  // Must not forget!
        std::cout << "Raw buffer manually deleted\n";
    }
    
    // Smart pointer approach
    {
        std::cout << "\nSmart pointer approach:\n";
        std::unique_ptr<int[]> smart_buffer = std::make_unique<int[]>(75);
        std::cout << "Smart buffer with unique_ptr\n";
    } // Automatically freed
}

/**
 * Demonstrate buffer operations
 */
void demonstrateBufferOperations() {
    std::cout << "\n\nBuffer Operations\n";
    std::cout << "=================\n";
    
    HeapBuffer<int> buffer(5);
    
    // Fill with data
    for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = static_cast<int>(i * 10);
    }
    
    // Print contents
    std::cout << "Buffer contents: ";
    for (size_t i = 0; i < buffer.size(); ++i) {
        std::cout << buffer[i] << " ";
    }
    std::cout << "\n";
    
    // Use pointer arithmetic (carefully)
    int* ptr = buffer.get();
    std::cout << "First element via pointer: " << *ptr << "\n";
    std::cout << "Second element via pointer: " << *(ptr + 1) << "\n";
    
    // Fill with pattern
    buffer.fill(99);
    std::cout << "After fill(99): ";
    for (size_t i = 0; i < buffer.size(); ++i) {
        std::cout << buffer[i] << " ";
    }
    std::cout << "\n";
}

/**
 * Main function
 */
int main() {
    try {
        // Configure global settings
        HeapBuffer<int>::config.max_allocation_size = 1024 * 1024;  // 1 MB
        HeapBuffer<int>::config.initialize_to_zero = true;
        HeapBuffer<int>::config.check_overflow = true;
        HeapBuffer<int>::config.enable_canary = true;
        
        std::cout << "Heap Buffer Configuration:\n";
        std::cout << "  Max allocation: " << HeapBuffer<int>::config.max_allocation_size << " bytes\n";
        std::cout << "  Initialize to zero: " << std::boolalpha 
                  << HeapBuffer<int>::config.initialize_to_zero << "\n";
        std::cout << "  Overflow checking: " << HeapBuffer<int>::config.check_overflow << "\n";
        std::cout << "  Canaries enabled: " << HeapBuffer<int>::config.enable_canary << "\n\n";
        
        demonstrateHeapAllocation();
        demonstrateErrorHandling();
        demonstrateCanaryProtection();
        demonstrateRawVsRAII();
        demonstrateBufferOperations();
        
        // Memory usage summary
        std::cout << "\n\nMemory Management Summary\n";
        std::cout << "=========================\n";
        std::cout << "✓ RAII ensures automatic cleanup\n";
        std::cout << "✓ Canaries detect buffer overflows\n";
        std::cout << "✓ Bounds checking prevents invalid access\n";
        std::cout << "✓ Move semantics prevent unnecessary copies\n";
        std::cout << "✓ Configurable allocation parameters\n";
        std::cout << "✓ Exception safety for allocation failures\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}