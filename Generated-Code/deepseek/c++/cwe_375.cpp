#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <shared_mutex>

/**
 * Custom exception for inventory errors
 */
class InventoryError : public std::runtime_error {
public:
    explicit InventoryError(const std::string& message) 
        : std::runtime_error("Inventory Error: " + message) {}
};

/**
 * Data object that will be encapsulated
 */
class DataObject {
private:
    std::string name;
    int quantity;
    double price;
    std::string category;
    std::vector<std::string> tags;
    std::map<std::string, std::string> metadata;
    
public:
    DataObject(const std::string& n = "default", int q = 0, double p = 0.0) 
        : name(n), quantity(q), price(p), category("uncategorized") {
        std::cout << "  [DataObject] Created: " << name << "\n";
    }
    
    // Copy constructor
    DataObject(const DataObject& other)
        : name(other.name), quantity(other.quantity), price(other.price),
          category(other.category), tags(other.tags), metadata(other.metadata) {
        std::cout << "  [DataObject] Copied: " << name << "\n";
    }
    
    ~DataObject() {
        std::cout << "  [DataObject] Destroyed: " << name << "\n";
    }
    
    // Getters
    std::string getName() const { return name; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    std::string getCategory() const { return category; }
    
    // Setters
    void setName(const std::string& n) { 
        std::cout << "  [DataObject] Setting name to: " << n << "\n";
        name = n; 
    }
    
    void setQuantity(int q) { 
        if (q < 0) throw InventoryError("Quantity cannot be negative");
        std::cout << "  [DataObject] Setting quantity to: " << q << "\n";
        quantity = q; 
    }
    
    void setPrice(double p) { 
        if (p < 0) throw InventoryError("Price cannot be negative");
        std::cout << "  [DataObject] Setting price to: " << p << "\n";
        price = p; 
    }
    
    void setCategory(const std::string& c) { 
        std::cout << "  [DataObject] Setting category to: " << c << "\n";
        category = c; 
    }
    
    void addTag(const std::string& tag) {
        tags.push_back(tag);
    }
    
    void setMetadata(const std::string& key, const std::string& value) {
        metadata[key] = value;
    }
    
    void print() const {
        std::cout << "  DataObject: " << name 
                  << " [Qty: " << quantity 
                  << ", Price: $" << std::fixed << std::setprecision(2) << price
                  << ", Category: " << category << "]\n";
        
        if (!tags.empty()) {
            std::cout << "    Tags: ";
            for (const auto& tag : tags) std::cout << tag << " ";
            std::cout << "\n";
        }
        
        if (!metadata.empty()) {
            std::cout << "    Metadata:\n";
            for (const auto& [key, val] : metadata) {
                std::cout << "      " << key << " : " << val << "\n";
            }
        }
    }
};

/**
 * DANGEROUS: Inventory class that returns non-const reference
 */
class Inventory {
private:
    DataObject item;  // Private data object
    
public:
    Inventory(const std::string& name, int qty, double price) 
        : item(name, qty, price) {
        std::cout << "[Inventory] Created with item: " << name << "\n";
    }
    
    // DANGEROUS: Returns non-const reference to private data
    DataObject& getItemUnsafe() {
        std::cout << "[Inventory] WARNING: Returning non-const reference!\n";
        return item;  // Bypasses encapsulation!
    }
    
    // Safer: Returns const reference (read-only)
    const DataObject& getItemReadOnly() const {
        std::cout << "[Inventory] Returning const reference\n";
        return item;
    }
    
    // Safer: Returns by value (copy)
    DataObject getItemCopy() const {
        std::cout << "[Inventory] Returning by value (copy)\n";
        return item;
    }
    
    // Best: Controlled access through methods
    void updateQuantity(int new_qty) {
        std::cout << "[Inventory] Controlled quantity update\n";
        if (new_qty < 0) {
            throw InventoryError("Quantity cannot be negative");
        }
        // Additional business logic could go here
        item.setQuantity(new_qty);
    }
    
    void updatePrice(double new_price) {
        std::cout << "[Inventory] Controlled price update\n";
        if (new_price < 0) {
            throw InventoryError("Price cannot be negative");
        }
        // Additional business logic could go here
        item.setPrice(new_price);
    }
    
    void print() const {
        std::cout << "[Inventory] Current state:\n";
        item.print();
    }
};

/**
 * Another example with more complex data
 */
class ComplexInventory {
private:
    std::map<std::string, DataObject> items;
    mutable std::shared_mutex mutex;
    std::string inventory_name;
    
public:
    ComplexInventory(const std::string& name) : inventory_name(name) {
        std::cout << "[ComplexInventory] Created: " << name << "\n";
    }
    
    // DANGEROUS: Returns non-const reference to internal map
    std::map<std::string, DataObject>& getItemsUnsafe() {
        std::cout << "[ComplexInventory] WARNING: Returning non-const reference to map!\n";
        return items;  // Complete encapsulation bypass!
    }
    
    // Slightly better but still dangerous for individual items
    DataObject& getItemUnsafe(const std::string& id) {
        std::cout << "[ComplexInventory] WARNING: Returning non-const reference to item!\n";
        return items[id];  // Creates item if not exists!
    }
    
    // Safer: Const reference to map
    const std::map<std::string, DataObject>& getItemsReadOnly() const {
        std::cout << "[ComplexInventory] Returning const reference to map\n";
        return items;
    }
    
    // Safer: Copy of map
    std::map<std::string, DataObject> getItemsCopy() const {
        std::cout << "[ComplexInventory] Returning copy of map\n";
        std::shared_lock lock(mutex);
        return items;
    }
    
    // Best: Controlled access with validation
    bool addItem(const std::string& id, const DataObject& obj) {
        std::unique_lock lock(mutex);
        if (items.find(id) != items.end()) {
            return false;  // Item already exists
        }
        items[id] = obj;
        return true;
    }
    
    bool updateItemQuantity(const std::string& id, int new_qty) {
        std::unique_lock lock(mutex);
        auto it = items.find(id);
        if (it == items.end()) {
            return false;
        }
        if (new_qty < 0) {
            throw InventoryError("Quantity cannot be negative");
        }
        it->second.setQuantity(new_qty);
        return true;
    }
    
    void print() const {
        std::shared_lock lock(mutex);
        std::cout << "[ComplexInventory: " << inventory_name << "]\n";
        for (const auto& [id, obj] : items) {
            std::cout << "  ID: " << id << "\n";
            obj.print();
        }
    }
};

/**
 * Demonstrate the dangers of non-const references
 */
void demonstrateDangers() {
    std::cout << "\n=== DANGERS of Non-Const Reference ===\n";
    
    Inventory inv("Laptop", 10, 999.99);
    std::cout << "\nInitial state:\n";
    inv.print();
    
    // DANGEROUS: Get non-const reference and modify directly
    std::cout << "\n1. Getting non-const reference and modifying...\n";
    DataObject& item_ref = inv.getItemUnsafe();
    item_ref.setQuantity(-5);  // Bypasses validation!
    item_ref.setPrice(-100.00);  // Bypasses validation!
    item_ref.setName("Hacked Laptop");
    
    std::cout << "\nAfter unsafe modification:\n";
    inv.print();
    
    // Even worse: Storing reference for later use
    std::cout << "\n2. Storing reference for later use...\n";
    static DataObject* global_ptr = nullptr;
    global_ptr = &item_ref;
    
    // Later in code...
    global_ptr->setQuantity(9999);  // Modifies inventory without its knowledge
    
    std::cout << "\nAfter modification through stored pointer:\n";
    inv.print();
}

/**
 * Demonstrate thread safety issues
 */
void demonstrateThreadSafety() {
    std::cout << "\n=== Thread Safety Issues ===\n";
    
    ComplexInventory store("Thread-Unsafe Store");
    
    // Get non-const reference
    auto& items = store.getItemsUnsafe();
    
    // In a multithreaded program, this would cause data races
    items["item1"] = DataObject("Thread-Unsafe Item", 1, 10.0);
    
    std::cout << "Store modified through reference\n";
    store.print();
}

/**
 * Demonstrate safe alternatives
 */
void demonstrateSafeAlternatives() {
    std::cout << "\n=== Safe Alternatives ===\n";
    
    Inventory inv("Monitor", 15, 299.99);
    inv.print();
    
    // Safe method 1: Const reference (read-only)
    std::cout << "\n1. Using const reference:\n";
    const auto& const_ref = inv.getItemReadOnly();
    std::cout << "  Read-only access: " << const_ref.getName() 
              << ", Qty: " << const_ref.getQuantity() << "\n";
    // const_ref.setQuantity(20);  // Compile error! Good.
    
    // Safe method 2: Copy
    std::cout << "\n2. Getting a copy:\n";
    DataObject copy = inv.getItemCopy();
    copy.setQuantity(100);  // Modifies copy only
    copy.setName("Modified Copy");
    
    std::cout << "  Original after copy modification:\n";
    inv.print();
    std::cout << "  Modified copy:\n";
    copy.print();
    
    // Safe method 3: Controlled updates
    std::cout << "\n3. Using controlled updates:\n";
    try {
        inv.updateQuantity(25);  // Valid update
        inv.updatePrice(279.99);  // Valid update
        
        // This would throw an exception
        // inv.updateQuantity(-10);  // Throws InventoryError
        // inv.updatePrice(-50.00);  // Throws InventoryError
    } catch (const InventoryError& e) {
        std::cout << "  Caught error: " << e.what() << "\n";
    }
    
    inv.print();
}

/**
 * Demonstrate best practices with complex inventory
 */
void demonstrateBestPractices() {
    std::cout << "\n=== Best Practices with Complex Inventory ===\n";
    
    ComplexInventory store("Best Store");
    
    // Controlled additions
    store.addItem("LAP001", DataObject("Gaming Laptop", 5, 1299.99));
    store.addItem("MON002", DataObject("4K Monitor", 8, 399.99));
    
    // Controlled updates
    store.updateItemQuantity("LAP001", 10);
    
    // Read-only access through const reference
    const auto& items = store.getItemsReadOnly();
    std::cout << "Store has " << items.size() << " items\n";
    
    // items["LAP001"].setQuantity(20);  // Compile error! Good.
    
    store.print();
    
    // Thread-safe copy
    auto items_copy = store.getItemsCopy();
    items_copy["NEW001"] = DataObject("New Item", 1, 9.99);  // Modifies copy
    
    std::cout << "\nOriginal unchanged:\n";
    store.print();
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Inventory Class - Non-Const Reference\n";
    std::cout << "========================================\n";
    
    try {
        demonstrateDangers();
        demonstrateThreadSafety();
        demonstrateSafeAlternatives();
        demonstrateBestPractices();
        
        // Comparison table
        std::cout << "\n=== Access Method Comparison ===\n";
        std::cout << std::left 
                  << std::setw(25) << "Method"
                  << std::setw(15) << "Thread-Safe"
                  << std::setw(15) << "Encapsulation"
                  << std::setw(15) << "Performance"
                  << "\n";
        std::cout << std::string(70, '-') << "\n";
        
        std::cout << std::setw(25) << "Non-const Reference"
                  << std::setw(15) << "❌ No"
                  << std::setw(15) << "❌ Poor"
                  << std::setw(15) << "Fast"
                  << "\n";
        
        std::cout << std::setw(25) << "Const Reference"
                  << std::setw(15) << "⚠️  Depends"
                  << std::setw(15) << "✓ Good"
                  << std::setw(15) << "Fast"
                  << "\n";
        
        std::cout << std::setw(25) << "Copy"
                  << std::setw(15) << "✓ Yes"
                  << std::setw(15) << "✓ Best"
                  << std::setw(15) << "Slow"
                  << "\n";
        
        std::cout << std::setw(25) << "Controlled Methods"
                  << std::setw(15) << "✓ Yes"
                  << std::setw(15) << "✓ Best"
                  << std::setw(15) << "Medium"
                  << "\n";
        
        // Best practices summary
        std::cout << "\n=== Key Takeaways ===\n";
        std::cout << "❌ NEVER return non-const references to private data\n";
        std::cout << "❌ NEVER expose internal data structures directly\n";
        std::cout << "✓ Use const references for read-only access\n";
        std::cout << "✓ Return copies when modifications are needed\n";
        std::cout << "✓ Provide controlled update methods\n";
        std::cout << "✓ Consider thread safety in design\n";
        std::cout << "✓ Maintain encapsulation for data integrity\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}