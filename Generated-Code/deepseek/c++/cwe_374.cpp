#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include <shared_mutex>
#include <mutex>

/**
 * Custom exception for configuration errors
 */
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) 
        : std::runtime_error("Configuration Error: " + message) {}
};

/**
 * SystemConfig class demonstrating different ways to expose a private map
 */
class SystemConfig {
private:
    // The private map
    std::map<std::string, std::string> config_map;
    
    // For thread-safe versions
    mutable std::shared_mutex rw_mutex;
    
    // Version tracking
    size_t version;
    
public:
    SystemConfig() : version(0) {
        // Initialize with some default values
        config_map["host"] = "localhost";
        config_map["port"] = "8080";
        config_map["timeout"] = "30";
        config_map["max_connections"] = "100";
        config_map["log_level"] = "info";
    }
    
    // ============= METHOD 1: UNSAFE - Returns raw pointer to private map =============
    // WARNING: This exposes internal data structure and bypasses encapsulation!
    std::map<std::string, std::string>* getMapPointer() {
        return &config_map;
    }
    
    // Const version - returns pointer to const map
    const std::map<std::string, std::string>* getMapPointer() const {
        return &config_map;
    }
    
    // ============= METHOD 2: Returns reference (slightly better) =============
    std::map<std::string, std::string>& getMapReference() {
        return config_map;
    }
    
    const std::map<std::string, std::string>& getMapReference() const {
        return config_map;
    }
    
    // ============= METHOD 3: Thread-safe access with locks =============
    std::map<std::string, std::string> getMapCopy() const {
        std::shared_lock<std::shared_mutex> lock(rw_mutex);
        return config_map;  // Returns a copy
    }
    
    // ============= METHOD 4: Iterator-based access (better encapsulation) =============
    using iterator = std::map<std::string, std::string>::iterator;
    using const_iterator = std::map<std::string, std::string>::const_iterator;
    
    iterator begin() { return config_map.begin(); }
    iterator end() { return config_map.end(); }
    const_iterator begin() const { return config_map.begin(); }
    const_iterator end() const { return config_map.end(); }
    const_iterator cbegin() const { return config_map.cbegin(); }
    const_iterator cend() const { return config_map.cend(); }
    
    // ============= METHOD 5: Key-based access (best encapsulation) =============
    std::string getValue(const std::string& key, const std::string& default_value = "") const {
        std::shared_lock<std::shared_mutex> lock(rw_mutex);
        auto it = config_map.find(key);
        if (it != config_map.end()) {
            return it->second;
        }
        return default_value;
    }
    
    void setValue(const std::string& key, const std::string& value) {
        std::unique_lock<std::shared_mutex> lock(rw_mutex);
        config_map[key] = value;
        version++;
    }
    
    bool hasKey(const std::string& key) const {
        std::shared_lock<std::shared_mutex> lock(rw_mutex);
        return config_map.find(key) != config_map.end();
    }
    
    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(rw_mutex);
        return config_map.size();
    }
    
    // ============= METHOD 6: View class for controlled access =============
    class ConfigView {
    private:
        const SystemConfig& config;
        std::shared_lock<std::shared_mutex> lock;
        
    public:
        explicit ConfigView(const SystemConfig& cfg) 
            : config(cfg), lock(cfg.rw_mutex) {}
        
        std::string get(const std::string& key, const std::string& default_val = "") const {
            auto it = config.config_map.find(key);
            if (it != config.config_map.end()) {
                return it->second;
            }
            return default_val;
        }
        
        size_t size() const { return config.config_map.size(); }
        
        bool contains(const std::string& key) const {
            return config.config_map.find(key) != config.config_map.end();
        }
        
        void print() const {
            std::cout << "ConfigView contents:\n";
            for (const auto& [key, value] : config.config_map) {
                std::cout << "  " << std::setw(15) << key << " : " << value << "\n";
            }
        }
    };
    
    ConfigView getView() const {
        return ConfigView(*this);
    }
    
    // ============= METHOD 7: Function object for queries =============
    template<typename Func>
    auto query(Func&& func) const -> decltype(func(config_map)) {
        std::shared_lock<std::shared_mutex> lock(rw_mutex);
        return func(config_map);
    }
    
    // ============= METHOD 8: Batch update with transaction =============
    class ConfigTransaction {
    private:
        SystemConfig& config;
        std::map<std::string, std::string> changes;
        bool committed;
        
    public:
        explicit ConfigTransaction(SystemConfig& cfg) 
            : config(cfg), committed(false) {}
        
        ~ConfigTransaction() {
            if (!committed) {
                std::cout << "Transaction destroyed without commit - changes lost\n";
            }
        }
        
        void update(const std::string& key, const std::string& value) {
            changes[key] = value;
        }
        
        void commit() {
            std::unique_lock<std::shared_mutex> lock(config.rw_mutex);
            for (const auto& [key, value] : changes) {
                config.config_map[key] = value;
            }
            config.version++;
            committed = true;
            std::cout << "Transaction committed with " << changes.size() << " changes\n";
        }
    };
    
    ConfigTransaction beginTransaction() {
        return ConfigTransaction(*this);
    }
    
    // ============= Helper methods =============
    void print() const {
        std::shared_lock<std::shared_mutex> lock(rw_mutex);
        std::cout << "\nSystemConfig (version " << version << "):\n";
        std::cout << "====================================\n";
        for (const auto& [key, value] : config_map) {
            std::cout << std::left << std::setw(20) << key << " : " << value << "\n";
        }
        std::cout << "====================================\n";
    }
    
    size_t getVersion() const { return version; }
};

/**
 * Demonstrate the dangers of returning raw pointers
 */
void demonstratePointerDangers() {
    std::cout << "\n=== DANGERS of Returning Raw Pointers ===\n";
    
    SystemConfig config;
    std::cout << "Initial config:\n";
    config.print();
    
    // UNSAFE: Get raw pointer to internal map
    std::map<std::string, std::string>* map_ptr = config.getMapPointer();
    
    std::cout << "\nModifying through raw pointer...\n";
    (*map_ptr)["host"] = "evil.com";  // Bypasses encapsulation
    (*map_ptr)["password"] = "hacked";  // Adds new key
    
    std::cout << "After external modification:\n";
    config.print();
    
    // Even worse: Pointer can be stored and used later
    std::cout << "\nStoring pointer for later use...\n";
    static std::map<std::string, std::string>* global_ptr = nullptr;
    global_ptr = map_ptr;
    
    // Later, somewhere else in code...
    (*global_ptr)["internal_secret"] = "exposed";
}

/**
 * Demonstrate thread safety issues
 */
void demonstrateThreadSafety() {
    std::cout << "\n=== Thread Safety Issues ===\n";
    
    SystemConfig config;
    
    // Get reference (not thread-safe!)
    std::map<std::string, std::string>& ref = config.getMapReference();
    
    // In a real multithreaded program, this would cause data races
    ref["thread_safe?"] = "no";
    
    std::cout << "Reference allows modification without locks\n";
}

/**
 * Demonstrate safe alternatives
 */
void demonstrateSafeAlternatives() {
    std::cout << "\n=== Safe Alternatives ===\n";
    
    SystemConfig config;
    
    // Method 1: Get a copy (thread-safe)
    std::cout << "\n1. Getting a copy:\n";
    auto config_copy = config.getMapCopy();
    config_copy["new_key"] = "copy_value";  // Modifies copy only
    
    std::cout << "Original config:\n";
    config.print();
    
    // Method 2: Use iterators
    std::cout << "\n2. Using iterators:\n";
    for (auto it = config.begin(); it != config.end(); ++it) {
        std::cout << "  " << it->first << " = " << it->second << "\n";
    }
    
    // Method 3: Key-based access
    std::cout << "\n3. Key-based access:\n";
    std::cout << "host = " << config.getValue("host") << "\n";
    std::cout << "port = " << config.getValue("port") << "\n";
    std::cout << "non_existent = " << config.getValue("non_existent", "default") << "\n";
    
    // Method 4: Use ConfigView
    std::cout << "\n4. Using ConfigView:\n";
    auto view = config.getView();
    view.print();
    
    // Method 5: Use query function
    std::cout << "\n5. Using query function:\n";
    auto keys = config.query([](const auto& map) {
        std::vector<std::string> result;
        for (const auto& [key, _] : map) {
            result.push_back(key);
        }
        return result;
    });
    
    std::cout << "Keys: ";
    for (const auto& key : keys) {
        std::cout << key << " ";
    }
    std::cout << "\n";
    
    // Method 6: Use transaction
    std::cout << "\n6. Using transaction:\n";
    {
        auto tx = config.beginTransaction();
        tx.update("new_key1", "value1");
        tx.update("new_key2", "value2");
        tx.commit();
    }
    
    config.print();
    
    // Transaction without commit
    {
        std::cout << "\nTransaction without commit:\n";
        auto tx = config.beginTransaction();
        tx.update("temp_key", "temp_value");
        // No commit - changes lost
    }
    
    config.print();
}

/**
 * Main demonstration
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "SystemConfig - Exposing Private Map\n";
    std::cout << "========================================\n";
    
    try {
        demonstratePointerDangers();
        demonstrateThreadSafety();
        demonstrateSafeAlternatives();
        
        // Comparison table
        std::cout << "\n=== Access Method Comparison ===\n";
        std::cout << std::left 
                  << std::setw(25) << "Method"
                  << std::setw(15) << "Thread-Safe"
                  << std::setw(15) << "Encapsulation"
                  << std::setw(15) << "Performance"
                  << "\n";
        std::cout << std::string(70, '-') << "\n";
        
        std::cout << std::setw(25) << "Raw Pointer"
                  << std::setw(15) << "❌ No"
                  << std::setw(15) << "❌ Poor"
                  << std::setw(15) << "Fast"
                  << "\n";
        
        std::cout << std::setw(25) << "Reference"
                  << std::setw(15) << "❌ No"
                  << std::setw(15) << "❌ Poor"
                  << std::setw(15) << "Fast"
                  << "\n";
        
        std::cout << std::setw(25) << "Copy"
                  << std::setw(15) << "✓ Yes"
                  << std::setw(15) << "✓ Good"
                  << std::setw(15) << "Slow"
                  << "\n";
        
        std::cout << std::setw(25) << "Iterators"
                  << std::setw(15) << "⚠️  Depends"
                  << std::setw(15) << "✓ Good"
                  << std::setw(15) << "Fast"
                  << "\n";
        
        std::cout << std::setw(25) << "Key-based Access"
                  << std::setw(15) << "✓ Yes"
                  << std::setw(15) << "✓ Best"
                  << std::setw(15) << "Medium"
                  << "\n";
        
        std::cout << std::setw(25) << "ConfigView"
                  << std::setw(15) << "✓ Yes"
                  << std::setw(15) << "✓ Best"
                  << std::setw(15) << "Medium"
                  << "\n";
        
        // Best practices
        std::cout << "\n=== Best Practices ===\n";
        std::cout << "❌ NEVER return raw pointers to internal data structures\n";
        std::cout << "❌ NEVER return non-const references to private data\n";
        std::cout << "✓ Prefer key-based access methods\n";
        std::cout << "✓ Use const-correctness\n";
        std::cout << "✓ Consider thread safety\n";
        std::cout << "✓ Provide view classes for complex access patterns\n";
        std::cout << "✓ Use transactions for batch updates\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}