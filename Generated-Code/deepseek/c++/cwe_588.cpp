#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <random>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

/**
 * Custom exception for type safety errors
 */
class TypeSafetyError : public std::runtime_error {
public:
    explicit TypeSafetyError(const std::string& message) 
        : std::runtime_error("Type Safety Error: " + message) {}
};

/**
 * Account structure - what we think the buffer contains
 */
struct Account {
    int id;
    char name[50];
    double balance;
    char account_type[20];
    bool is_active;
    
    void print() const {
        std::cout << "Account[ID=" << id 
                  << ", Name=" << name 
                  << ", Balance=" << balance 
                  << ", Type=" << account_type 
                  << ", Active=" << (is_active ? "Yes" : "No") 
                  << "]\n";
    }
};

/**
 * Different structure - what the buffer might actually contain
 */
struct Transaction {
    int transaction_id;
    char description[100];
    double amount;
    long timestamp;
    char status[10];
    
    void print() const {
        std::cout << "Transaction[ID=" << transaction_id 
                  << ", Desc=" << description 
                  << ", Amount=" << amount 
                  << ", Time=" << timestamp 
                  << ", Status=" << status 
                  << "]\n";
    }
};

/**
 * Another different structure
 */
struct Customer {
    int customer_id;
    char full_name[100];
    char email[50];
    char phone[20];
    int loyalty_points;
    
    void print() const {
        std::cout << "Customer[ID=" << customer_id 
                  << ", Name=" << full_name 
                  << ", Email=" << email 
                  << ", Phone=" << phone 
                  << ", Points=" << loyalty_points 
                  << "]\n";
    }
};

/**
 * DANGEROUS: Casts void* to Account* without checking
 */
void processBufferAsAccount(void* buffer) {
    std::cout << "\n=== DANGEROUS: Processing buffer as Account ===\n";
    
    if (!buffer) {
        std::cerr << "Null buffer provided\n";
        return;
    }
    
    // DANGEROUS: No type checking, just casting
    Account* acc = static_cast<Account*>(buffer);
    
    std::cout << "Cast Account* at: " << static_cast<void*>(acc) << "\n";
    std::cout << "Accessing as Account:\n";
    
    // This will interpret whatever is in the buffer as an Account
    acc->print();
}

/**
 * DANGEROUS: Another version with reinterpret_cast
 */
void processBufferWithReinterpretCast(void* buffer) {
    std::cout << "\n=== DANGEROUS: Using reinterpret_cast ===\n";
    
    if (!buffer) return;
    
    // reinterpret_cast is even more dangerous
    Account* acc = reinterpret_cast<Account*>(buffer);
    
    std::cout << "Interpreting buffer as Account:\n";
    std::cout << "ID: " << acc->id << "\n";
    std::cout << "Name: " << acc->name << "\n";
    std::cout << "Balance: " << acc->balance << "\n";
}

/**
 * SAFE: Check type before casting
 */
class TypedBuffer {
private:
    void* data;
    size_t size;
    std::type_index type;
    
public:
    template<typename T>
    static TypedBuffer create(const T& value) {
        T* buffer = new T(value);
        return TypedBuffer(buffer, sizeof(T), typeid(T));
    }
    
    TypedBuffer(void* d, size_t s, const std::type_info& t) 
        : data(d), size(s), type(t) {}
    
    ~TypedBuffer() {
        if (data) {
            // Need to know type to delete properly
            // This is simplified - real code would need type-aware deletion
            std::cout << "Buffer destroyed (type: " << type.name() << ")\n";
            free(data);  // Simplified - not safe for non-POD
        }
    }
    
    template<typename T>
    T* getAs() {
        if (type != typeid(T)) {
            throw TypeSafetyError("Buffer type mismatch: expected " + 
                                  std::string(typeid(T).name()) + 
                                  ", got " + std::string(type.name()));
        }
        return static_cast<T*>(data);
    }
    
    std::type_index getType() const { return type; }
};

/**
 * SAFE: Use union for type-safe variant
 */
union DataVariant {
    Account acc;
    Transaction trans;
    Customer cust;
    
    DataVariant() {}  // No initialization
    ~DataVariant() {} // No destruction - be careful with non-POD
};

enum class DataType { ACCOUNT, TRANSACTION, CUSTOMER };

class SafeVariant {
private:
    DataVariant data;
    DataType type;
    
public:
    explicit SafeVariant(const Account& a) : type(DataType::ACCOUNT) {
        new (&data.acc) Account(a);
    }
    
    explicit SafeVariant(const Transaction& t) : type(DataType::TRANSACTION) {
        new (&data.trans) Transaction(t);
    }
    
    explicit SafeVariant(const Customer& c) : type(DataType::CUSTOMER) {
        new (&data.cust) Customer(c);
    }
    
    ~SafeVariant() {
        // Properly destruct based on type
        switch (type) {
            case DataType::ACCOUNT:
                data.acc.~Account();
                break;
            case DataType::TRANSACTION:
                data.trans.~Transaction();
                break;
            case DataType::CUSTOMER:
                data.cust.~Customer();
                break;
        }
    }
    
    Account* getAsAccount() {
        if (type != DataType::ACCOUNT) {
            throw TypeSafetyError("Variant does not contain Account");
        }
        return &data.acc;
    }
    
    Transaction* getAsTransaction() {
        if (type != DataType::TRANSACTION) {
            throw TypeSafetyError("Variant does not contain Transaction");
        }
        return &data.trans;
    }
    
    Customer* getAsCustomer() {
        if (type != DataType::CUSTOMER) {
            throw TypeSafetyError("Variant does not contain Customer");
        }
        return &data.cust;
    }
};

/**
 * Demonstrate the dangers of unsafe casting
 */
void demonstrateDangers() {
    std::cout << "========================================\n";
    std::cout << "Unsafe Void* Casting Demonstration\n";
    std::cout << "========================================\n";
    
    // Case 1: Correct type (works as expected)
    {
        std::cout << "\n--- Case 1: Correct type ---\n";
        Account correct_account = {1001, "John Doe", 5000.0, "Checking", true};
        
        void* buffer = &correct_account;
        processBufferAsAccount(buffer);  // Works correctly
    }
    
    // Case 2: Wrong type (undefined behavior)
    {
        std::cout << "\n--- Case 2: Wrong type (Transaction as Account) ---\n";
        Transaction wrong_type = {5001, "Payment", 299.99, 1234567890, "Pending"};
        
        void* buffer = &wrong_type;
        processBufferAsAccount(buffer);  // UNDEFINED BEHAVIOR!
        std::cout << "⚠️  This may print garbage or crash\n";
    }
    
    // Case 3: Wrong type (Customer as Account)
    {
        std::cout << "\n--- Case 3: Wrong type (Customer as Account) ---\n";
        Customer customer = {2001, "Alice Smith", "alice@email.com", "555-0123", 1500};
        
        void* buffer = &customer;
        processBufferAsAccount(buffer);  // UNDEFINED BEHAVIOR!
    }
    
    // Case 4: Misaligned data
    {
        std::cout << "\n--- Case 4: Misaligned data ---\n";
        char misaligned_buffer[sizeof(Account) + 1];
        char* offset_buffer = misaligned_buffer + 1;  // Misaligned
        
        Account* original = new(offset_buffer) Account{2002, "Misaligned", 1000.0, "Savings", true};
        
        void* buffer = offset_buffer;
        processBufferAsAccount(buffer);  // May crash on some architectures
    }
}

/**
 * Demonstrate memory corruption
 */
void demonstrateCorruption() {
    std::cout << "\n=== Memory Corruption Example ===\n";
    
    // Create a valid account
    Account valid_account = {3001, "Important Data", 1000000.0, "VIP", true};
    
    std::cout << "Original account:\n";
    valid_account.print();
    
    // Create a transaction in adjacent memory
    char memory_pool[sizeof(Account) + sizeof(Transaction)];
    memcpy(memory_pool, &valid_account, sizeof(Account));
    
    Transaction* transaction = reinterpret_cast<Transaction*>(memory_pool + sizeof(Account));
    *transaction = {9999, "Malicious Transaction", -999999.99, 12345, "HACKED"};
    
    // Now treat the whole pool as Account (dangerous!)
    std::cout << "\nTreating memory as Account after corruption:\n";
    Account* corrupted = reinterpret_cast<Account*>(memory_pool);
    corrupted->print();  // May show corrupted data
    
    // Modify account through wrong type
    std::cout << "\nModifying through wrong type:\n";
    Transaction* as_trans = reinterpret_cast<Transaction*>(&valid_account);
    as_trans->amount = 0.01;  // Corrupting the account balance through wrong type
    
    std::cout << "Account after corruption:\n";
    valid_account.print();  // Balance may be corrupted
}

/**
 * SAFE: Type-erased wrapper with type checking
 */
class TypeSafeBuffer {
private:
    void* data;
    std::type_index stored_type;
    size_t size;
    std::string type_name;
    
public:
    template<typename T>
    explicit TypeSafeBuffer(const T& value) 
        : stored_type(typeid(T))
        , size(sizeof(T))
        , type_name(typeid(T).name()) {
        
        data = new T(value);
        std::cout << "[Safe] Created buffer of type " << type_name << "\n";
    }
    
    ~TypeSafeBuffer() {
        // Can't safely delete without knowing type
        // In real code, use a type-erased deleter
        std::cout << "[Safe] Buffer destroyed (type: " << type_name << ")\n";
        free(data);  // Simplified - not safe for non-POD
    }
    
    template<typename T>
    T* get() {
        if (stored_type != typeid(T)) {
            throw TypeSafetyError("Type mismatch: requested " + 
                                  std::string(typeid(T).name()) + 
                                  ", but buffer contains " + type_name);
        }
        return static_cast<T*>(data);
    }
    
    std::type_index getType() const { return stored_type; }
};

/**
 * SAFE: Virtual base class approach
 */
class BufferInterface {
public:
    virtual ~BufferInterface() = default;
    virtual void print() const = 0;
    virtual std::string getTypeName() const = 0;
};

template<typename T>
class TypedBufferInterface : public BufferInterface {
private:
    T data;
    
public:
    explicit TypedBufferInterface(const T& d) : data(d) {}
    
    void print() const override {
        std::cout << "Buffer contains: ";
        if constexpr (std::is_same_v<T, Account>) {
            data.print();
        } else if constexpr (std::is_same_v<T, Transaction>) {
            data.print();
        } else if constexpr (std::is_same_v<T, Customer>) {
            data.print();
        }
    }
    
    std::string getTypeName() const override {
        return typeid(T).name();
    }
    
    T* get() { return &data; }
};

/**
 * Demonstrate safe alternatives
 */
void demonstrateSafeAlternatives() {
    std::cout << "\n=== Safe Alternatives ===\n";
    
    // Safe alternative 1: Type-safe buffer
    {
        std::cout << "\n--- Type-safe buffer ---\n";
        TypeSafeBuffer safe_account(Account{4001, "Safe User", 7500.0, "Premium", true});
        
        try {
            auto* acc = safe_account.get<Account>();
            std::cout << "Got Account successfully:\n";
            acc->print();
            
            // This would throw
            // auto* trans = safe_account.get<Transaction>();
            
        } catch (const TypeSafetyError& e) {
            std::cout << "Caught type error: " << e.what() << "\n";
        }
    }
    
    // Safe alternative 2: Virtual interface
    {
        std::cout << "\n--- Virtual interface ---\n";
        std::vector<std::unique_ptr<BufferInterface>> buffers;
        
        buffers.push_back(std::make_unique<TypedBufferInterface<Account>>(
            Account{5001, "Virtual User", 3000.0, "Standard", true}));
        buffers.push_back(std::make_unique<TypedBufferInterface<Transaction>>(
            Transaction{6001, "Virtual Transaction", 150.50, 987654321, "Completed"}));
        
        for (const auto& buf : buffers) {
            std::cout << "Buffer type: " << buf->getTypeName() << "\n";
            buf->print();
        }
    }
    
    // Safe alternative 3: Variant
    {
        std::cout << "\n--- Variant approach ---\n";
        SafeVariant var1(Account{7001, "Variant User", 10000.0, "Gold", true});
        SafeVariant var2(Transaction{8001, "Variant Transaction", 75.25, 1122334455, "Pending"});
        
        try {
            auto* acc = var1.getAsAccount();
            std::cout << "Variant 1 is Account:\n";
            acc->print();
            
            // This will throw
            // auto* trans = var1.getAsTransaction();
            
        } catch (const TypeSafetyError& e) {
            std::cout << "Caught expected error: " << e.what() << "\n";
        }
    }
}

/**
 * Main function
 */
int main() {
    try {
        demonstrateDangers();
        demonstrateCorruption();
        demonstrateSafeAlternatives();
        
        // Summary
        std::cout << "\n=== Summary ===\n";
        std::cout << "❌ NEVER cast void* to a type without checking\n";
        std::cout << "✓ Use type-safe containers instead\n";
        std::cout << "✓ Store type information with the buffer\n";
        std::cout << "✓ Use virtual interfaces for polymorphic behavior\n";
        std::cout << "✓ Consider std::variant for fixed type sets\n";
        std::cout << "✓ Always validate before casting\n";
        
        std::cout << "\n=== Undefined Behavior Consequences ===\n";
        std::cout << "1. Reading wrong data (interpretation error)\n";
        std::cout << "2. Memory corruption when writing\n";
        std::cout << "3. Alignment faults (crashes)\n";
        std::cout << "4. Security vulnerabilities\n";
        std::cout << "5. Hard-to-debug intermittent failures\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}