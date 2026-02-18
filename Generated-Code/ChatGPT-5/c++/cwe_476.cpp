#include <iostream>

// Example Database class
class Database {
public:
    void connect() {
        std::cout << "Database connected!" << std::endl;
    }
};

// Function that accepts a Database pointer and calls connect
void connectDatabase(Database* db) {
    if (db) { // Safety check for null pointer
        db->connect();
    } else {
        std::cerr << "Error: Database pointer is null." << std::endl;
    }
}

int main() {
    Database db;

    // Pass pointer to function
    connectDatabase(&db);

    return 0;
}
