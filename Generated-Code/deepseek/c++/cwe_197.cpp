#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <random>
#include <bitset>

/**
 * Custom exception for ID truncation errors
 */
class IDTruncationError : public std::runtime_error {
public:
    explicit IDTruncationError(const std::string& message) 
        : std::runtime_error("ID Truncation Error: " + message) {}
};

/**
 * Configuration for ID truncation
 */
struct TruncationConfig {
    enum class CollisionStrategy {
        THROW_EXCEPTION,           // Throw on any collision
        RETURN_COLLISION_FLAG,      // Return collision indicator
        USE_HASHING,                // Use hash function for distribution
        USE_MODULO,                 // Use modulo operation
        USE_BITMASK                  // Use bitmasking
    };
    
    CollisionStrategy collision_strategy;
    bool check_collision;            // Whether to check for collisions
    bool preserve_uniqueness;         // Attempt to preserve uniqueness
    uint16_t modulo_value;             // For modulo strategy
    uint16_t bitmask;                   // For bitmask strategy
    std::function<uint16_t(uint64_t)> hash_function; // Custom hash
    
    TruncationConfig() 
        : collision_strategy(CollisionStrategy::RETURN_COLLISION_FLAG)
        , check_collision(true)
        , preserve_uniqueness(false)
        , modulo_value(65535)  // 2^16 - 1
        , bitmask(0xFFFF)       // Lower 16 bits
        , hash_function(nullptr) {}
};

/**
 * Result structure for truncation operation
 */
struct TruncationResult {
    uint16_t truncated_id;
    uint64_t original_id;
    bool collision_detected;
    bool truncated;
    uint64_t collision_count;
    std::string warning_message;
    
    TruncationResult() : truncated_id(0), original_id(0), 
                         collision_detected(false), truncated(false),
                         collision_count(0) {}
};

/**
 * ID Truncator class with various strategies
 */
class IDTruncator {
private:
    TruncationConfig config;
    std::unordered_map<uint16_t, std::vector<uint64_t>> collision_map;
    
    /**
     * Strategy 1: Direct truncation (lower 16 bits)
     */
    uint16_t truncateDirect(uint64_t id) const {
        return static_cast<uint16_t>(id & 0xFFFF);
    }
    
    /**
     * Strategy 2: Modulo truncation
     */
    uint16_t truncateModulo(uint64_t id) const {
        return static_cast<uint16_t>(id % config.modulo_value);
    }
    
    /**
     * Strategy 3: Bitmask truncation
     */
    uint16_t truncateBitmask(uint64_t id) const {
        return static_cast<uint16_t>(id & config.bitmask);
    }
    
    /**
     * Strategy 4: Hash-based truncation
     */
    uint16_t truncateHash(uint64_t id) const {
        if (config.hash_function) {
            return config.hash_function(id);
        }
        
        // Default hash: XOR folding
        uint32_t upper = static_cast<uint32_t>(id >> 32);
        uint32_t lower = static_cast<uint32_t>(id);
        uint32_t mixed = upper ^ lower;
        return static_cast<uint16_t>((mixed >> 16) ^ (mixed & 0xFFFF));
    }
    
    /**
     * Check for collisions
     */
    bool checkCollision(uint16_t truncated, uint64_t original) {
        auto& ids = collision_map[truncated];
        
        // Check if this original ID already exists
        for (uint64_t existing : ids) {
            if (existing == original) {
                return false;  // Same ID, not a collision
            }
        }
        
        // Different ID, this is a collision
        ids.push_back(original);
        return ids.size() > 1;
    }
    
    /**
     * Generate collision report
     */
    std::string generateCollisionReport(uint16_t truncated, uint64_t original) {
        std::stringstream ss;
        ss << "Collision detected: multiple 64-bit IDs map to 16-bit value " 
           << truncated << " (0x" << std::hex << std::setw(4) << std::setfill('0') 
           << truncated << std::dec << ")\n";
        
        const auto& ids = collision_map[truncated];
        ss << "  Conflicting IDs:\n";
        for (uint64_t id : ids) {
            ss << "    - " << id << " (0x" << std::hex << id << std::dec << ")\n";
        }
        
        return ss.str();
    }
    
public:
    explicit IDTruncator(const TruncationConfig& cfg = TruncationConfig{}) 
        : config(cfg) {}
    
    /**
     * Main truncation function
     */
    TruncationResult truncate(uint64_t user_id) {
        TruncationResult result;
        result.original_id = user_id;
        
        // Apply selected truncation strategy
        uint16_t truncated;
        
        switch (config.collision_strategy) {
            case TruncationConfig::CollisionStrategy::USE_MODULO:
                truncated = truncateModulo(user_id);
                result.truncated = true;
                break;
                
            case TruncationConfig::CollisionStrategy::USE_BITMASK:
                truncated = truncateBitmask(user_id);
                result.truncated = true;
                break;
                
            case TruncationConfig::CollisionStrategy::USE_HASHING:
                truncated = truncateHash(user_id);
                result.truncated = true;
                break;
                
            case TruncationConfig::CollisionStrategy::THROW_EXCEPTION:
            case TruncationConfig::CollisionStrategy::RETURN_COLLISION_FLAG:
            default:
                truncated = truncateDirect(user_id);
                result.truncated = true;
                break;
        }
        
        result.truncated_id = truncated;
        
        // Check for collisions if enabled
        if (config.check_collision) {
            bool collision = checkCollision(truncated, user_id);
            result.collision_detected = collision;
            
            if (collision) {
                result.collision_count = collision_map[truncated].size() - 1;
                result.warning_message = generateCollisionReport(truncated, user_id);
                
                // Handle collision based on strategy
                switch (config.collision_strategy) {
                    case TruncationConfig::CollisionStrategy::THROW_EXCEPTION:
                        throw IDTruncationError(result.warning_message);
                        
                    case TruncationConfig::CollisionStrategy::RETURN_COLLISION_FLAG:
                        // Return with collision flag set
                        break;
                        
                    default:
                        // Other strategies continue with collision warning
                        break;
                }
            }
        }
        
        return result;
    }
    
    /**
     * Batch truncation
     */
    std::vector<TruncationResult> truncateBatch(const std::vector<uint64_t>& ids) {
        std::vector<TruncationResult> results;
        results.reserve(ids.size());
        
        for (uint64_t id : ids) {
            results.push_back(truncate(id));
        }
        
        return results;
    }
    
    /**
     * Get collision statistics
     */
    void printCollisionStats() const {
        std::cout << "\nCollision Statistics:\n";
        std::cout << "=====================\n";
        
        size_t total_collisions = 0;
        size_t bins_with_collisions = 0;
        
        for (const auto& [truncated, ids] : collision_map) {
            if (ids.size() > 1) {
                bins_with_collisions++;
                total_collisions += ids.size() - 1;
                std::cout << "Bin " << std::setw(5) << truncated 
                          << " (0x" << std::hex << std::setw(4) << std::setfill('0') 
                          << truncated << std::dec << "): " 
                          << ids.size() << " IDs\n";
            }
        }
        
        std::cout << "\nTotal bins with collisions: " << bins_with_collisions << "\n";
        std::cout << "Total collisions: " << total_collisions << "\n";
        std::cout << "Collision rate: " 
                  << std::fixed << std::setprecision(2)
                  << (total_collisions * 100.0 / collision_map.size()) << "%\n";
    }
    
    /**
     * Reset collision tracking
     */
    void resetTracking() {
        collision_map.clear();
    }
};

/**
 * Demonstrate different truncation strategies
 */
void demonstrateStrategies() {
    std::cout << "\n=== Truncation Strategy Comparison ===\n";
    
    // Test IDs with different patterns
    std::vector<uint64_t> test_ids = {
        0x0000000000000001,  // Small number
        0x000000000000FFFF,  // Lower 16 bits all 1
        0xFFFF000000000000,  // Upper 16 bits all 1
        0x123456789ABCDEF0,  // Random pattern
        0xAAAAAAAAAAAAAAAA,  // Alternating bits
        0xFFFFFFFFFFFFFFFF,  // All ones
        0x0000FFFF0000FFFF,  // Repeating pattern
        0xDEADBEEFDEADBEEF,  // Known pattern
        0x0000000100000001,  // Repeated 1
        0x0000000000000000   // Zero
    };
    
    // Strategy 1: Direct truncation (lower 16 bits)
    {
        std::cout << "\nStrategy 1: Direct truncation (lower 16 bits):\n";
        IDTruncator direct_truncator;
        
        for (uint64_t id : test_ids) {
            auto result = direct_truncator.truncate(id);
            std::cout << "  0x" << std::hex << std::setw(16) << std::setfill('0') << id
                      << " -> 0x" << std::setw(4) << result.truncated_id 
                      << " (" << std::dec << result.truncated_id << ")\n";
        }
    }
    
    // Strategy 2: Modulo truncation
    {
        std::cout << "\nStrategy 2: Modulo 10000:\n";
        TruncationConfig config;
        config.collision_strategy = TruncationConfig::CollisionStrategy::USE_MODULO;
        config.modulo_value = 10000;
        
        IDTruncator modulo_truncator(config);
        
        for (uint64_t id : test_ids) {
            auto result = modulo_truncator.truncate(id);
            std::cout << "  0x" << std::hex << std::setw(16) << std::setfill('0') << id
                      << " -> " << std::dec << std::setw(5) << result.truncated_id << "\n";
        }
    }
    
    // Strategy 3: Hash-based truncation
    {
        std::cout << "\nStrategy 3: XOR hash:\n";
        TruncationConfig config;
        config.collision_strategy = TruncationConfig::CollisionStrategy::USE_HASHING;
        
        IDTruncator hash_truncator(config);
        
        for (uint64_t id : test_ids) {
            auto result = hash_truncator.truncate(id);
            std::cout << "  0x" << std::hex << std::setw(16) << std::setfill('0') << id
                      << " -> 0x" << std::setw(4) << result.truncated_id << "\n";
        }
    }
    
    // Strategy 4: Bitmask truncation
    {
        std::cout << "\nStrategy 4: Bitmask 0x0FFF (12 bits):\n";
        TruncationConfig config;
        config.collision_strategy = TruncationConfig::CollisionStrategy::USE_BITMASK;
        config.bitmask = 0x0FFF;
        
        IDTruncator bitmask_truncator(config);
        
        for (uint64_t id : test_ids) {
            auto result = bitmask_truncator.truncate(id);
            std::cout << "  0x" << std::hex << std::setw(16) << std::setfill('0') << id
                      << " -> 0x" << std::setw(3) << result.truncated_id << "\n";
        }
    }
}

/**
 * Demonstrate collision detection
 */
void demonstrateCollisionDetection() {
    std::cout << "\n=== Collision Detection ===\n";
    
    // Generate IDs that will cause collisions
    std::vector<uint64_t> colliding_ids = {
        0x0000000000001234,
        0xFFFF000000001234,  // Different upper bits, same lower 16 bits
        0x1234000000001234,  // Same lower 16 bits
        0xABCD000000001234,  // Same lower 16 bits
        0x0000000000005678,
        0xFFFF000000005678   // Another collision group
    };
    
    TruncationConfig config;
    config.check_collision = true;
    config.collision_strategy = TruncationConfig::CollisionStrategy::RETURN_COLLISION_FLAG;
    
    IDTruncator truncator(config);
    
    for (uint64_t id : colliding_ids) {
        auto result = truncator.truncate(id);
        
        std::cout << "ID: 0x" << std::hex << std::setw(16) << std::setfill('0') << id
                  << " -> 0x" << std::setw(4) << result.truncated_id << std::dec;
        
        if (result.collision_detected) {
            std::cout << " ⚠️  COLLISION DETECTED!\n";
            std::cout << "  " << result.warning_message << "\n";
        } else {
            std::cout << "\n";
        }
    }
    
    truncator.printCollisionStats();
}

/**
 * Demonstrate exception handling
 */
void demonstrateExceptionHandling() {
    std::cout << "\n=== Exception Handling ===\n";
    
    TruncationConfig config;
    config.check_collision = true;
    config.collision_strategy = TruncationConfig::CollisionStrategy::THROW_EXCEPTION;
    
    IDTruncator truncator(config);
    
    std::vector<uint64_t> ids = {
        0x000000000000AAAA,
        0xFFFF00000000AAAA   // This will cause collision
    };
    
    for (uint64_t id : ids) {
        try {
            auto result = truncator.truncate(id);
            std::cout << "Success: 0x" << std::hex << id 
                      << " -> 0x" << result.truncated_id << std::dec << "\n";
        } catch (const IDTruncationError& e) {
            std::cout << "Caught exception: " << e.what() << "\n";
        }
    }
}

/**
 * Demonstrate collision probability
 */
void demonstrateCollisionProbability() {
    std::cout << "\n=== Collision Probability ===\n";
    
    // Generate random IDs
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;
    
    TruncationConfig config;
    config.check_collision = true;
    
    std::cout << "Testing collision rates with random IDs:\n";
    
    std::vector<size_t> test_sizes = {100, 1000, 10000, 20000, 30000, 40000, 50000, 60000, 65535};
    
    for (size_t num_ids : test_sizes) {
        IDTruncator truncator(config);
        size_t collisions = 0;
        
        for (size_t i = 0; i < num_ids; ++i) {
            uint64_t id = dist(gen);
            auto result = truncator.truncate(id);
            if (result.collision_detected) {
                collisions++;
            }
        }
        
        double probability = static_cast<double>(collisions) / num_ids * 100;
        std::cout << "  IDs: " << std::setw(6) << num_ids 
                  << " | Collisions: " << std::setw(4) << collisions
                  << " | Probability: " << std::fixed << std::setprecision(2)
                  << std::setw(6) << probability << "%\n";
    }
    
    // Theoretical probability (birthday problem)
    std::cout << "\nTheoretical collision probability (birthday problem):\n";
    std::cout << "  For 100 IDs: " << std::setprecision(4) 
              << (1 - exp(-(100.0 * 99) / (2 * 65536))) * 100 << "%\n";
    std::cout << "  For 1000 IDs: " 
              << (1 - exp(-(1000.0 * 999) / (2 * 65536))) * 100 << "%\n";
    std::cout << "  For 10000 IDs: " 
              << (1 - exp(-(10000.0 * 9999) / (2 * 65536))) * 100 << "%\n";
}

/**
 * Database index simulation
 */
class DatabaseIndexSimulator {
private:
    std::unordered_map<uint16_t, std::vector<uint64_t>> index;
    IDTruncator truncator;
    size_t total_collisions;
    
public:
    DatabaseIndexSimulator() : total_collisions(0) {}
    
    bool insertUser(uint64_t user_id) {
        auto result = truncator.truncate(user_id);
        
        if (result.collision_detected) {
            total_collisions++;
            std::cout << "Warning: User ID " << user_id 
                      << " collides with existing ID(s) in index " 
                      << result.truncated_id << "\n";
        }
        
        index[result.truncated_id].push_back(user_id);
        return !result.collision_detected;
    }
    
    std::vector<uint64_t> lookupByIndex(uint16_t idx) {
        auto it = index.find(idx);
        if (it != index.end()) {
            return it->second;
        }
        return {};
    }
    
    void printStats() {
        std::cout << "\nDatabase Index Statistics:\n";
        std::cout << "  Total unique indices: " << index.size() << "\n";
        std::cout << "  Total collisions: " << total_collisions << "\n";
        
        size_t max_chain = 0;
        uint16_t max_idx = 0;
        
        for (const auto& [idx, users] : index) {
            if (users.size() > max_chain) {
                max_chain = users.size();
                max_idx = idx;
            }
        }
        
        std::cout << "  Longest chain: " << max_chain << " users at index " << max_idx << "\n";
    }
};

/**
 * Main demonstration
 */
int main() {
    try {
        std::cout << "===========================================\n";
        std::cout << "64-bit to 16-bit ID Truncation for Database\n";
        std::cout << "===========================================\n";
        
        // Basic truncation function (simple version)
        auto truncate_simple = [](uint64_t user_id) -> uint16_t {
            return static_cast<uint16_t>(user_id & 0xFFFF);
        };
        
        std::cout << "\nSimple truncation example:\n";
        std::cout << "User ID 0x123456789ABCDEF0 -> Index " 
                  << std::hex << "0x" << truncate_simple(0x123456789ABCDEF0) << std::dec << "\n";
        
        // Demonstrate different strategies
        demonstrateStrategies();
        
        // Show collision detection
        demonstrateCollisionDetection();
        
        // Exception handling demo
        demonstrateExceptionHandling();
        
        // Probability analysis
        demonstrateCollisionProbability();
        
        // Database simulation
        std::cout << "\n=== Database Index Simulation ===\n";
        DatabaseIndexSimulator db;
        
        // Insert some users
        std::vector<uint64_t> sample_users = {
            0x1000000000000001,
            0x2000000000000001,  // Collides with first
            0x3000000000000002,
            0x4000000000000002,  // Collides with third
            0x5000000000000003,
            0x6000000000000004,
            0x7000000000000003,  // Collides with fifth
            0x8000000000000005,
            0x9000000000000004,  // Collides with sixth
            0xA000000000000006
        };
        
        for (uint64_t user : sample_users) {
            std::cout << "Inserting user 0x" << std::hex << user << std::dec << "... ";
            if (db.insertUser(user)) {
                std::cout << "OK\n";
            } else {
                std::cout << "Collision\n";
            }
        }
        
        db.printStats();
        
        // Lookup example
        std::cout << "\nLooking up index 1:\n";
        auto users = db.lookupByIndex(1);
        for (uint64_t user : users) {
            std::cout << "  Found user 0x" << std::hex << user << std::dec << "\n";
        }
        
        // Summary
        std::cout << "\n=== Summary ===\n";
        std::cout << "✓ Multiple truncation strategies available\n";
        std::cout << "✓ Collision detection and handling\n";
        std::cout << "✓ Configurable behavior for collisions\n";
        std::cout << "✓ Statistical analysis of collision probability\n";
        std::cout << "✓ Database index simulation\n";
        std::cout << "\nNote: " << (65536) << " possible 16-bit indices for " 
                  << "2^64 possible user IDs\n";
        std::cout << "Collisions are mathematically certain for large datasets\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}