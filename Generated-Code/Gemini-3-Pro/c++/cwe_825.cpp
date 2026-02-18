#include <iostream>
#include <vector>
#include <string>

// A simple structure to represent a user
struct User {
    std::string name;
    User(std::string n) : name(n) {}
    ~User() { std::cout << "[Destructor] User object destroyed in memory.\n"; }
};

// 1. The Global List
std::vector<User*> global_user_registry;

void register_temporary_user() {
    // 2. Allocate an object locally on the stack
    User local_user("admin_alice");
    
    std::cout << "Registering user: " << local_user.name << "\n";

    // 3. THE VULNERABILITY: Storing the address of a local variable globally
    global_user_registry.push_back(&local_user);

    // 4. The function ends. 'local_user' is instantly destroyed.
}

int main() {
    std::cout << "--- Starting Registration ---\n";
    register_temporary_user();
    
    std::cout << "\n--- Attempting Unsafe Access ---\n";
    
    // 5. THE CRASH: The global list holds a dangling pointer.
    if (!global_user_registry.empty()) {
        User* dangling_ptr = global_user_registry[0];
        
        std::cout << "Warning: Dereferencing a dead pointer...\n";
        
        // This will likely cause a Segmentation Fault. The std::string internal 
        // buffer was already freed by the destructor in step 4.
        std::cout << "Accessed Name: " << dangling_ptr->name << "\n";
    }

    return 0;
}