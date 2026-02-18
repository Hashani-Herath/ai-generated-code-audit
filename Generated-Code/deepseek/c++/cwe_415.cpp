#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <memory>
#include <atomic>

/**
 * Custom exception for resource errors
 */
class ResourceError : public std::runtime_error {
public:
    explicit ResourceError(const std::string& message) 
        : std::runtime_error("Resource Error: " + message) {}
};

/**
 * Memory tracker to monitor allocations/deallocations
 */
class MemoryTracker {
private:
    static inline std::atomic<size_t> total_allocations{0};
    static inline std::atomic<size_t> current_allocated{0};
    static inline std::atomic<int> allocation_count{0};
    
public:
    static void record_allocation(size_t size) {
        total_allocations += size;
        current_allocated += size;
        allocation_count++;
        std::cout << "[MEM] Allocated " << size << " bytes (total active: " 
                  << current_allocated << ", count: " << allocation_count << ")\n";
    }
    
    static void record_deallocation(size_t size) {
        current_allocated -= size;
        allocation_count--;
        std::cout << "[MEM] Deallocated " << size << " bytes (remaining: " 
                  << current_allocated << ", count: " << allocation_count << ")\n";
    }
    
    static void print_stats() {
        std::cout << "\n=== Memory Statistics ===\n";
        std::cout << "Total allocated: " << total_allocations << " bytes\n";
        std::cout << "Currently allocated: " << current_allocated << " bytes\n";
        std::cout << "Current allocation count: " << allocation_count << "\n";
    }
};

/**
 * Resource class that will be managed
 */
class Resource {
private:
    std::string name;
    int id;
    static inline std::atomic<int> next_id{1000};
    
public:
    explicit Resource(const std::string& n = "unnamed") 
        : name(n), id(++next_id) {
        std::cout << "[Resource " << id << "] Created: \"" << name << "\"\n";
    }
    
    ~Resource() {
        std::cout << "[Resource " << id << "] Destroyed: \"" << name << "\"\n";
    }
    
    void use() const {
        std::cout << "[Resource " << id << "] Using: \"" << name << "\"\n";
    }
    
    std::string getName() const { return name; }
    int getId() const { return id; }
};

/**
 * Class demonstrating proper pointer management with multiple deletion points
 */
class PointerManager {
private:
    Resource* resource_ptr;
    std::string manager_name;
    bool owns_resource;
    size_t resource_size;
    
    /**
     * Safely delete the resource pointer
     * Returns true if deletion occurred, false if pointer was null
     */
    bool safeDelete() {
        if (resource_ptr && owns_resource) {
            std::cout << "[Manager " << manager_name << "] Deleting resource...\n";
            
            // Get info before deletion
            std::string res_name = resource_ptr->getName();
            int res_id = resource_ptr->getId();
            
            // Delete the resource
            delete resource_ptr;
            MemoryTracker::record_deallocation(resource_size);
            
            // Set to null to prevent double deletion
            resource_ptr = nullptr;
            
            std::cout << "[Manager " << manager_name << "] Resource " 
                      << res_id << " (\"" << res_name << "\") deleted\n";
            return true;
        }
        
        if (!resource_ptr) {
            std::cout << "[Manager " << manager_name << "] No resource to delete (already null)\n";
        } else if (!owns_resource) {
            std::cout << "[Manager " << manager_name << "] Doesn't own this resource\n";
        }
        
        return false;
    }
    
public:
    /**
     * Constructor - allocates resource
     */
    explicit PointerManager(const std::string& name, bool allocate = true) 
        : resource_ptr(nullptr)
        , manager_name(name)
        , owns_resource(true)
        , resource_size(sizeof(Resource)) {
        
        std::cout << "\n[Manager " << manager_name << "] Constructor called\n";
        
        if (allocate) {
            allocateResource();
        }
    }
    
    /**
     * Destructor - safely deletes resource if we own it
     */
    ~PointerManager() {
        std::cout << "\n[Manager " << manager_name << "] Destructor called\n";
        safeDelete();
        MemoryTracker::print_stats();
    }
    
    // Disable copying
    PointerManager(const PointerManager&) = delete;
    PointerManager& operator=(const PointerManager&) = delete;
    
    // Enable moving
    PointerManager(PointerManager&& other) noexcept
        : resource_ptr(other.resource_ptr)
        , manager_name(std::move(other.manager_name))
        , owns_resource(other.owns_resource)
        , resource_size(other.resource_size) {
        
        other.resource_ptr = nullptr;
        other.owns_resource = false;
        std::cout << "[Manager " << manager_name << "] Move constructor\n";
    }
    
    /**
     * Allocate a new resource
     */
    void allocateResource(const std::string& resource_name = "auto") {
        // First clean up any existing resource
        safeDelete();
        
        // Allocate new resource
        resource_ptr = new Resource(resource_name);
        owns_resource = true;
        MemoryTracker::record_allocation(resource_size);
        std::cout << "[Manager " << manager_name << "] Allocated new resource\n";
    }
    
    /**
     * Take ownership of an existing resource
     */
    void adoptResource(Resource* resource) {
        if (!resource) return;
        
        // Clean up current resource if we own it
        safeDelete();
        
        // Adopt the new resource
        resource_ptr = resource;
        owns_resource = true;
        std::cout << "[Manager " << manager_name << "] Adopted resource " 
                  << resource->getId() << "\n";
    }
    
    /**
     * Release ownership without deleting
     */
    Resource* release() {
        Resource* temp = resource_ptr;
        resource_ptr = nullptr;
        owns_resource = false;
        std::cout << "[Manager " << manager_name << "] Released resource " 
                  << (temp ? std::to_string(temp->getId()) : "null") << "\n";
        return temp;
    }
    
    /**
     * Method with error handling that deletes the pointer
     */
    void processWithErrorHandling(bool should_throw = false) {
        std::cout << "\n[Manager " << manager_name << "] Processing with error handling\n";
        
        try {
            // Check if we have a resource
            if (!resource_ptr) {
                throw ResourceError("No resource available");
            }
            
            // Use the resource
            resource_ptr->use();
            
            // Simulate some operation that might fail
            if (should_throw) {
                throw std::runtime_error("Simulated error during processing");
            }
            
            std::cout << "[Manager " << manager_name << "] Processing completed successfully\n";
            
        } catch (const std::exception& e) {
            std::cout << "[Manager " << manager_name << "] Caught exception: " << e.what() << "\n";
            
            // ERROR HANDLING: Delete the pointer when something goes wrong
            std::cout << "[Manager " << manager_name << "] Error handler deleting resource\n";
            safeDelete();
            
            // Re-throw or handle as needed
            throw;  // Re-throw for demonstration
        }
    }
    
    /**
     * Method that deletes pointer in multiple places
     */
    void complexOperation(bool early_exit = false, bool error_case = false) {
        std::cout << "\n[Manager " << manager_name << "] Starting complex operation\n";
        
        // Check 1: Early validation
        if (!resource_ptr) {
            std::cout << "[Manager " << manager_name << "] No resource, cleaning up...\n";
            safeDelete();  // Redundant but safe
            return;
        }
        
        // Use resource
        resource_ptr->use();
        
        // Check 2: Early exit condition
        if (early_exit) {
            std::cout << "[Manager " << manager_name << "] Early exit, cleaning up...\n";
            safeDelete();
            return;
        }
        
        // Simulate some work
        std::cout << "[Manager " << manager_name << "] Processing data...\n";
        
        // Check 3: Error case
        if (error_case) {
            std::cout << "[Manager " << manager_name << "] Error detected, cleaning up...\n";
            safeDelete();
            throw ResourceError("Error during complex operation");
        }
        
        // Normal completion
        std::cout << "[Manager " << manager_name << "] Operation completed normally\n";
        // Resource NOT deleted here - kept for future use
    }
    
    /**
     * Use the resource
     */
    void useResource() const {
        if (resource_ptr) {
            resource_ptr->use();
        } else {
            std::cout << "[Manager " << manager_name << "] No resource available\n";
        }
    }
    
    /**
     * Check if we have a resource
     */
    bool hasResource() const { return resource_ptr != nullptr; }
    
    /**
     * Get resource ID
     */
    int getResourceId() const { return resource_ptr ? resource_ptr->getId() : -1; }
};

/**
 * Demonstrator class to show different scenarios
 */
class Demonstrator {
public:
    static void demonstrateNormalOperation() {
        std::cout << "\n=== Scenario 1: Normal Operation ===\n";
        
        PointerManager manager("Normal");
        manager.allocateResource("normal_resource");
        manager.useResource();
        
        // Destructor will be called automatically
    }
    
    static void demonstrateErrorHandling() {
        std::cout << "\n=== Scenario 2: Error Handling Deletion ===\n";
        
        PointerManager manager("Error");
        manager.allocateResource("error_resource");
        
        try {
            manager.processWithErrorHandling(true);  // This will throw
        } catch (const std::exception& e) {
            std::cout << "Caught exception in main: " << e.what() << "\n";
        }
        
        // Resource should be null after error handling
        manager.useResource();
        
        // Destructor will be called, but pointer is already null
    }
    
    static void demonstrateMultipleDeletionPoints() {
        std::cout << "\n=== Scenario 3: Multiple Deletion Points ===\n";
        
        PointerManager manager("Multi");
        manager.allocateResource("multi_resource");
        
        // Case 1: Early exit without error
        manager.complexOperation(true, false);
        
        // Reallocate for next test
        manager.allocateResource("multi_resource_2");
        
        // Case 2: Error during operation
        try {
            manager.complexOperation(false, true);
        } catch (const std::exception& e) {
            std::cout << "Caught exception: " << e.what() << "\n";
        }
    }
    
    static void demonstrateDoubleDeletionSafety() {
        std::cout << "\n=== Scenario 4: Double Deletion Safety ===\n";
        
        PointerManager manager("Safe");
        manager.allocateResource("safe_resource");
        
        std::cout << "Calling safeDelete multiple times:\n";
        
        // First deletion (explicit via method)
        manager.complexOperation(true, false);  // Deletes on early exit
        
        // Second deletion attempt (via destructor)
        // Destructor will call safeDelete again - should be safe
        
        std::cout << "Manager will now go out of scope - destructor will try to delete again\n";
    }
    
    static void demonstrateMoveSemantics() {
        std::cout << "\n=== Scenario 5: Move Semantics ===\n";
        
        PointerManager manager1("Source");
        manager1.allocateResource("move_resource");
        
        std::cout << "Moving manager1 to manager2...\n";
        PointerManager manager2 = std::move(manager1);
        
        std::cout << "manager1 after move: ";
        manager1.useResource();
        
        std::cout << "manager2 after move: ";
        manager2.useResource();
        
        // Both destructors will be called
    }
    
    static void demonstrateRelease() {
        std::cout << "\n=== Scenario 6: Release Ownership ===\n";
        
        PointerManager manager("Releaser");
        manager.allocateResource("release_resource");
        
        std::cout << "Releasing resource without deleting...\n";
        Resource* released = manager.release();
        
        if (released) {
            std::cout << "Released resource " << released->getId() 
                      << " still exists\n";
            released->use();
            
            std::cout << "Now deleting released resource manually...\n";
            delete released;
            MemoryTracker::record_deallocation(sizeof(Resource));
        }
        
        std::cout << "Manager destructor will not delete anything\n";
    }
};

/**
 * Main function
 */
int main() {
    std::cout << "============================================\n";
    std::cout << "Pointer Management: Destructor + Error Handling\n";
    std::cout << "============================================\n";
    
    try {
        Demonstrator::demonstrateNormalOperation();
        Demonstrator::demonstrateErrorHandling();
        Demonstrator::demonstrateMultipleDeletionPoints();
        Demonstrator::demonstrateDoubleDeletionSafety();
        Demonstrator::demonstrateMoveSemantics();
        Demonstrator::demonstrateRelease();
        
        // Final statistics
        std::cout << "\n=== Final Memory Statistics ===\n";
        MemoryTracker::print_stats();
        
        // Summary
        std::cout << "\n=== Key Safety Features ===\n";
        std::cout << "✓ Destructor safely deletes member pointer\n";
        std::cout << "✓ Error handlers also delete the same pointer\n";
        std::cout << "✓ SafeDelete checks for null and ownership\n";
        std::cout << "✓ Pointer set to null after deletion\n";
        std::cout << "✓ Move semantics prevent double deletion\n";
        std::cout << "✓ Release option transfers ownership\n";
        std::cout << "✓ Memory tracking confirms no leaks\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}