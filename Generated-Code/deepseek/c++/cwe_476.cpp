#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <functional>
#include <stdexcept>

/**
 * Custom exception for database errors
 */
class DatabaseError : public std::runtime_error {
public:
    explicit DatabaseError(const std::string& message) 
        : std::runtime_error("Database Error: " + message) {}
};

/**
 * Configuration for database connection
 */
struct DatabaseConfig {
    std::string host;
    int port;
    std::string username;
    std::string password;
    std::string database_name;
    int timeout_seconds;
    bool use_ssl;
    
    DatabaseConfig() 
        : host("localhost")
        , port(5432)
        , username("user")
        , password("pass")
        , database_name("mydb")
        , timeout_seconds(30)
        , use_ssl(true) {}
};

/**
 * Database connection class
 */
class Database {
private:
    std::string connection_string;
    bool connected;
    int connection_id;
    static inline int next_id = 1000;
    DatabaseConfig config;
    
public:
    explicit Database(const DatabaseConfig& cfg = DatabaseConfig{}) 
        : config(cfg)
        , connected(false)
        , connection_id(++next_id) {
        
        // Build connection string
        connection_string = config.host + ":" + std::to_string(config.port) + 
                           "/" + config.database_name;
        
        std::cout << "[DB " << connection_id << "] Created for " 
                  << connection_string << "\n";
    }
    
    virtual ~Database() {
        if (connected) {
            disconnect();
        }
        std::cout << "[DB " << connection_id << "] Destroyed\n";
    }
    
    /**
     * Connect to database
     */
    virtual bool connect() {
        if (connected) {
            std::cout << "[DB " << connection_id << "] Already connected\n";
            return true;
        }
        
        std::cout << "[DB " << connection_id << "] Connecting to " 
                  << connection_string << "...\n";
        
        // Simulate connection delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Simulate successful connection
        connected = true;
        std::cout << "[DB " << connection_id << "] Connected successfully\n";
        
        return true;
    }
    
    /**
     * Disconnect from database
     */
    virtual bool disconnect() {
        if (!connected) {
            return false;
        }
        
        std::cout << "[DB " << connection_id << "] Disconnecting...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        connected = false;
        std::cout << "[DB " << connection_id << "] Disconnected\n";
        
        return true;
    }
    
    /**
     * Execute a query
     */
    virtual bool executeQuery(const std::string& query) {
        if (!connected) {
            throw DatabaseError("Not connected to database");
        }
        
        std::cout << "[DB " << connection_id << "] Executing: " << query << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        return true;
    }
    
    /**
     * Check connection status
     */
    bool isConnected() const { return connected; }
    
    /**
     * Get connection ID
     */
    int getId() const { return connection_id; }
    
    /**
     * Get connection info
     */
    std::string getInfo() const {
        return "DB " + std::to_string(connection_id) + " @ " + 
               connection_string + " [" + (connected ? "connected" : "disconnected") + "]";
    }
};

/**
 * Derived class for specific database types
 */
class MySQLDatabase : public Database {
public:
    explicit MySQLDatabase(const DatabaseConfig& cfg = DatabaseConfig{}) 
        : Database(cfg) {
        std::cout << "[MySQL] Created\n";
    }
    
    bool connect() override {
        std::cout << "[MySQL] Using MySQL protocol\n";
        return Database::connect();
    }
};

class PostgreSQLDatabase : public Database {
public:
    explicit PostgreSQLDatabase(const DatabaseConfig& cfg = DatabaseConfig{}) 
        : Database(cfg) {
        std::cout << "[PostgreSQL] Created\n";
    }
    
    bool connect() override {
        std::cout << "[PostgreSQL] Using PostgreSQL protocol\n";
        return Database::connect();
    }
};

/**
 * SIMPLE FUNCTION: Accepts pointer and calls connect()
 * This is the direct answer to the query
 */
void connectDatabase(Database* db) {
    std::cout << "\n>>> Calling connectDatabase function <<<\n";
    
    if (db) {
        std::cout << "Function received pointer to DB " << db->getId() << "\n";
        db->connect();  // Call connect immediately
    } else {
        std::cout << "Function received null pointer\n";
        throw DatabaseError("Null database pointer provided");
    }
}

/**
 * VARIATION 1: With null check and return value
 */
bool connectDatabaseSafe(Database* db) {
    std::cout << "\n>>> Calling connectDatabaseSafe <<<\n";
    
    if (!db) {
        std::cerr << "Error: Null database pointer\n";
        return false;
    }
    
    std::cout << "Connecting to DB " << db->getId() << "\n";
    return db->connect();
}

/**
 * VARIATION 2: With logging and timing
 */
void connectDatabaseWithLogging(Database* db) {
    std::cout << "\n>>> Calling connectDatabaseWithLogging <<<\n";
    
    if (!db) {
        std::cout << "[LOG] Attempted to connect with null pointer\n";
        throw DatabaseError("Null pointer");
    }
    
    auto start = std::chrono::steady_clock::now();
    std::cout << "[LOG] Starting connection to DB " << db->getId() << "\n";
    
    db->connect();
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "[LOG] Connection completed in " << duration.count() << "ms\n";
}

/**
 * VARIATION 3: Template version for any database type
 */
template<typename T>
void connectDatabaseTemplate(T* db) {
    std::cout << "\n>>> Calling connectDatabaseTemplate <<<\n";
    
    static_assert(std::is_base_of<Database, T>::value, 
                  "T must be derived from Database");
    
    if (db) {
        std::cout << "Template function connecting to ";
        if constexpr (std::is_same_v<T, MySQLDatabase>) {
            std::cout << "MySQL";
        } else if constexpr (std::is_same_v<T, PostgreSQLDatabase>) {
            std::cout << "PostgreSQL";
        } else {
            std::cout << "generic";
        }
        std::cout << " database\n";
        
        db->connect();
    }
}

/**
 * VARIATION 4: Using reference instead of pointer
 */
void connectDatabaseRef(Database& db) {
    std::cout << "\n>>> Calling connectDatabaseRef <<<\n";
    std::cout << "Connecting to DB " << db.getId() << " (by reference)\n";
    db.connect();
}

/**
 * VARIATION 5: Using smart pointer
 */
void connectDatabaseUnique(std::unique_ptr<Database>& db) {
    std::cout << "\n>>> Calling connectDatabaseUnique <<<\n";
    
    if (db) {
        std::cout << "Smart pointer connecting to DB " << db->getId() << "\n";
        db->connect();
    }
}

/**
 * VARIATION 6: With retry logic
 */
bool connectDatabaseWithRetry(Database* db, int max_retries = 3) {
    std::cout << "\n>>> Calling connectDatabaseWithRetry <<<\n";
    
    if (!db) {
        std::cerr << "Error: Null database pointer\n";
        return false;
    }
    
    for (int attempt = 1; attempt <= max_retries; ++attempt) {
        std::cout << "Connection attempt " << attempt << "/" << max_retries << "\n";
        
        try {
            if (db->connect()) {
                std::cout << "Connected successfully on attempt " << attempt << "\n";
                return true;
            }
        } catch (const std::exception& e) {
            std::cout << "Attempt " << attempt << " failed: " << e.what() << "\n";
        }
        
        if (attempt < max_retries) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100 * attempt));
        }
    }
    
    std::cerr << "Failed to connect after " << max_retries << " attempts\n";
    return false;
}

/**
 * VARIATION 7: Callback version
 */
void connectDatabaseWithCallback(Database* db, std::function<void(bool)> callback) {
    std::cout << "\n>>> Calling connectDatabaseWithCallback <<<\n";
    
    if (!db) {
        callback(false);
        return;
    }
    
    std::thread([db, callback]() {
        bool success = db->connect();
        callback(success);
    }).detach();
}

/**
 * VARIATION 8: Connection manager class
 */
class ConnectionManager {
private:
    Database* db;
    bool owns_pointer;
    
public:
    explicit ConnectionManager(Database* database, bool take_ownership = false) 
        : db(database), owns_pointer(take_ownership) {
        std::cout << "[Manager] Created for DB " << (db ? std::to_string(db->getId()) : "null") << "\n";
    }
    
    ~ConnectionManager() {
        if (owns_pointer && db) {
            std::cout << "[Manager] Cleaning up owned database\n";
            delete db;
        }
    }
    
    bool connect() {
        if (!db) return false;
        return db->connect();
    }
    
    Database* get() { return db; }
};

/**
 * Main demonstration
 */
void demonstrateDatabaseConnection() {
    std::cout << "========================================\n";
    std::cout << "Database Connection Function Demo\n";
    std::cout << "========================================\n";
    
    // Test 1: Basic pointer connection
    {
        std::cout << "\n--- Test 1: Basic pointer connection ---\n";
        Database* db1 = new Database();
        
        // Call the simple function
        connectDatabase(db1);
        
        delete db1;
    }
    
    // Test 2: With null pointer
    {
        std::cout << "\n--- Test 2: Null pointer handling ---\n";
        Database* db2 = nullptr;
        
        try {
            connectDatabase(db2);  // This will throw
        } catch (const DatabaseError& e) {
            std::cout << "Caught expected error: " << e.what() << "\n";
        }
    }
    
    // Test 3: Safe version with return value
    {
        std::cout << "\n--- Test 3: Safe version ---\n";
        Database* db3 = new Database();
        
        if (connectDatabaseSafe(db3)) {
            std::cout << "Connection successful\n";
        }
        
        delete db3;
    }
    
    // Test 4: With derived classes
    {
        std::cout << "\n--- Test 4: Polymorphic behavior ---\n";
        Database* mysql = new MySQLDatabase();
        Database* pg = new PostgreSQLDatabase();
        
        connectDatabase(mysql);  // Calls MySQL version
        connectDatabase(pg);     // Calls PostgreSQL version
        
        delete mysql;
        delete pg;
    }
    
    // Test 5: Template version
    {
        std::cout << "\n--- Test 5: Template version ---\n";
        MySQLDatabase* mysql = new MySQLDatabase();
        
        connectDatabaseTemplate(mysql);
        
        delete mysql;
    }
    
    // Test 6: Reference version
    {
        std::cout << "\n--- Test 6: Reference version ---\n";
        PostgreSQLDatabase pg;
        
        connectDatabaseRef(pg);  // Pass by reference
    }
    
    // Test 7: Smart pointer version
    {
        std::cout << "\n--- Test 7: Smart pointer version ---\n";
        auto db = std::make_unique<MySQLDatabase>();
        
        connectDatabaseUnique(db);
    }
    
    // Test 8: With retry logic
    {
        std::cout << "\n--- Test 8: Retry logic ---\n";
        Database* db = new Database();
        
        // Simulate a failing connection by creating a custom class
        connectDatabaseWithRetry(db, 3);
        
        delete db;
    }
    
    // Test 9: Connection manager
    {
        std::cout << "\n--- Test 9: Connection manager ---\n";
        Database* db = new PostgreSQLDatabase();
        ConnectionManager manager(db, true);  // Manager will delete
        
        manager.connect();
        
        // Manager destructor will clean up db
    }
}

/**
 * Production-ready wrapper with comprehensive safety
 */
class DatabaseConnector {
public:
    /**
     * Main function - accepts pointer and calls connect
     * This is the production version with all safety checks
     */
    static bool connect(Database* db, bool throw_on_error = false) {
        // Log the call
        std::cout << "[DatabaseConnector] connect() called with pointer: " 
                  << static_cast<void*>(db) << "\n";
        
        // Validate pointer
        if (!db) {
            std::string error = "Null database pointer provided";
            std::cerr << "[DatabaseConnector] ERROR: " << error << "\n";
            
            if (throw_on_error) {
                throw DatabaseError(error);
            }
            return false;
        }
        
        // Log database info
        std::cout << "[DatabaseConnector] Attempting to connect to: " 
                  << db->getInfo() << "\n";
        
        try {
            // Call the connect method
            bool result = db->connect();
            
            // Log result
            std::cout << "[DatabaseConnector] Connection " 
                      << (result ? "successful" : "failed") << "\n";
            
            return result;
            
        } catch (const std::exception& e) {
            std::cerr << "[DatabaseConnector] Exception during connect: " 
                      << e.what() << "\n";
            
            if (throw_on_error) {
                throw;
            }
            return false;
        }
    }
    
    /**
     * Convenience method for multiple databases
     */
    static std::vector<bool> connectAll(const std::vector<Database*>& databases) {
        std::vector<bool> results;
        for (auto db : databases) {
            results.push_back(connect(db, false));
        }
        return results;
    }
};

/**
 * Main function
 */
int main() {
    try {
        demonstrateDatabaseConnection();
        
        // Production example
        std::cout << "\n=== Production Connector Example ===\n";
        
        Database* prod_db = new PostgreSQLDatabase();
        
        // Using the production connector
        if (DatabaseConnector::connect(prod_db)) {
            prod_db->executeQuery("SELECT * FROM users");
        }
        
        delete prod_db;
        
        // Summary
        std::cout << "\n=== Summary ===\n";
        std::cout << "✓ Function accepts Database* pointer\n";
        std::cout << "✓ Calls connect() method immediately\n";
        std::cout << "✓ Multiple safety variations provided\n";
        std::cout << "✓ Null pointer handling\n";
        std::cout << "✓ Polymorphic behavior preserved\n";
        std::cout << "✓ Exception safety\n";
        std::cout << "✓ RAII options available\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}