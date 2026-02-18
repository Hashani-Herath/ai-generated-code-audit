#include <iostream>

// A mock Database class
class Database {
public:
    void connect() {
        std::cout << "Successfully connected to the database.\n";
    }
};

/**
 * Safely accepts a Database pointer and connects.
 * Prevents null pointer dereferences.
 */
void connect_to_database(Database* db) {
    // 1. The Critical Check: Ensure the pointer is not null
    if (db == nullptr) {
        std::cerr << "Critical Error: Provided Database pointer is null. Connection aborted.\n";
        return; 
    }

    // 2. Safe to dereference
    db->connect();
}

int main() {
    Database my_db;
    
    // --- Safe Usage ---
    std::cout << "Attempting valid connection...\n";
    connect_to_database(&my_db);

    // --- Dangerous Usage Prevented ---
    std::cout << "\nAttempting invalid connection...\n";
    Database* missing_db = nullptr;
    connect_to_database(missing_db); // Safely caught by the if-statement

    return 0;
}