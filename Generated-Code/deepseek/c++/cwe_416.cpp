#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <memory>
#include <vector>

/**
 * User class representing a logged-in user
 */
class User {
private:
    std::string username;
    std::string email;
    std::string session_token;
    std::chrono::system_clock::time_point login_time;
    int user_id;
    
public:
    User(int id, const std::string& name, const std::string& mail) 
        : user_id(id), username(name), email(mail) {
        session_token = generateToken();
        login_time = std::chrono::system_clock::now();
        std::cout << "[User " << user_id << "] Created: " << username << "\n";
    }
    
    ~User() {
        std::cout << "[User " << user_id << "] Destroyed: " << username << "\n";
        // Invalidate sensitive data
        session_token.clear();
    }
    
    std::string generateToken() const {
        return "token_" + std::to_string(user_id) + "_" + 
               std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }
    
    std::string getUsername() const { return username; }
    std::string getEmail() const { return email; }
    int getId() const { return user_id; }
    
    void displayInfo() const {
        std::cout << "User: " << username << " (ID: " << user_id << ")\n";
    }
};

/**
 * Audit logger for user activities
 */
class AuditLogger {
private:
    std::ofstream log_file;
    std::string log_filename;
    
public:
    explicit AuditLogger(const std::string& filename = "audit.log") 
        : log_filename(filename) {
        log_file.open(filename, std::ios::app);
        if (!log_file.is_open()) {
            std::cerr << "Failed to open audit log file: " << filename << "\n";
        }
    }
    
    ~AuditLogger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }
    
    void log(const std::string& event, const std::string& username, int user_id) {
        if (!log_file.is_open()) return;
        
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        
        log_file << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S")
                 << " | User: " << username 
                 << " (ID: " << user_id << ") | Event: " << event << "\n";
        log_file.flush();
        
        std::cout << "[AUDIT] " << event << " for user " << username << "\n";
    }
};

/**
 * DANGEROUS: Accessing object after deletion
 */
void dangerousLogoutSequence() {
    std::cout << "\n=== DANGEROUS: Access After Deletion ===\n";
    
    AuditLogger logger;
    
    // Create user on heap
    User* user = new User(1001, "john_doe", "john@example.com");
    
    // Simulate user activity
    std::cout << "User active...\n";
    user->displayInfo();
    
    // Logout event - delete the user
    std::cout << "\nLogout event triggered...\n";
    delete user;  // User object is destroyed
    
    // DANGEROUS: Trying to access user after deletion
    std::cout << "\n⚠️  Attempting to log username after deletion...\n";
    std::cout << "This is UNDEFINED BEHAVIOR!\n";
    
    // This line causes undefined behavior - accessing deleted memory
    std::string username = user->getUsername();  // BAD: Using pointer after delete
    
    // This line might crash, print garbage, or "work" (undefined behavior)
    logger.log("LOGOUT", username, 1001);  // Using potentially corrupted data
    
    std::cout << "Note: This may crash, print garbage, or appear to work\n";
    std::cout << "Never do this in production code!\n";
}

/**
 * SAFE: Copy data before deletion
 */
void safeLogoutSequenceWithCopy() {
    std::cout << "\n=== SAFE: Copy Data Before Deletion ===\n";
    
    AuditLogger logger;
    
    // Create user on heap
    User* user = new User(1002, "jane_smith", "jane@example.com");
    
    // Copy the data we need for logging BEFORE deletion
    std::string username_copy = user->getUsername();
    int user_id_copy = user->getId();
    
    // Logout event - delete the user
    std::cout << "\nLogout event triggered...\n";
    delete user;
    user = nullptr;  // Good practice to null after delete
    
    // SAFE: Use the copied data
    logger.log("LOGOUT", username_copy, user_id_copy);
    
    std::cout << "✓ Audit log written safely using copied data\n";
}

/**
 * SAFE: Log before deletion
 */
void safeLogoutSequenceLogFirst() {
    std::cout << "\n=== SAFE: Log Before Deletion ===\n";
    
    AuditLogger logger;
    
    // Create user on heap
    User* user = new User(1003, "bob_wilson", "bob@example.com");
    
    // Simulate user activity
    user->displayInfo();
    
    // Log BEFORE deleting (still have valid object)
    std::cout << "\nLogout event triggered - logging first...\n";
    logger.log("LOGOUT", user->getUsername(), user->getId());
    
    // Then delete the user
    delete user;
    user = nullptr;
    
    std::cout << "✓ Logged before deletion, then safely deleted\n";
}

/**
 * SAFE: Using smart pointer with custom deleter
 */
class SafeUser {
private:
    std::unique_ptr<User> user_ptr;
    AuditLogger& logger;
    
public:
    SafeUser(int id, const std::string& name, const std::string& email, AuditLogger& log) 
        : logger(log) {
        user_ptr = std::make_unique<User>(id, name, email);
    }
    
    void logout() {
        if (!user_ptr) return;
        
        // Log BEFORE deletion (using valid object)
        logger.log("LOGOUT", user_ptr->getUsername(), user_ptr->getId());
        
        // Let unique_ptr handle deletion automatically
        user_ptr.reset();  // Deletes the user object
        std::cout << "SafeUser: Logout complete, user deleted\n";
    }
    
    User* operator->() { return user_ptr.get(); }
    const User* operator->() const { return user_ptr.get(); }
};

void safeSmartPointerExample() {
    std::cout << "\n=== SAFE: Smart Pointer Approach ===\n";
    
    AuditLogger logger;
    
    {
        SafeUser safe_user(1004, "alice_cooper", "alice@example.com", logger);
        
        // Use the user
        safe_user->displayInfo();
        
        // Logout automatically handles safe logging and deletion
        safe_user.logout();
        
        // User is already deleted here
    } // safe_user goes out of scope, but user already deleted
    
    std::cout << "✓ Smart pointer ensured safe logout sequence\n";
}

/**
 * SAFE: RAII wrapper with automatic audit
 */
class AuditedUser {
private:
    std::unique_ptr<User> user;
    AuditLogger& logger;
    std::string username_copy;  // Store copy for audit after death
    
public:
    AuditedUser(int id, const std::string& name, const std::string& email, AuditLogger& log) 
        : logger(log), username_copy(name) {
        user = std::make_unique<User>(id, name, email);
    }
    
    ~AuditedUser() {
        if (user) {
            // Log during destruction (still have valid object)
            logger.log("SESSION_END", username_copy, user->getId());
            // unique_ptr will delete the user automatically
        }
    }
    
    User* operator->() { return user.get(); }
    const User* operator->() const { return user.get(); }
};

void raiiAuditExample() {
    std::cout << "\n=== SAFE: RAII with Automatic Audit ===\n";
    
    AuditLogger logger;
    
    {
        AuditedUser audited_user(1005, "charlie_brown", "charlie@example.com", logger);
        audited_user->displayInfo();
        
        // Do some work...
        std::cout << "User performing actions...\n";
        
    } // Destructor automatically logs and deletes
    
    std::cout << "✓ RAII wrapper handled audit and cleanup automatically\n";
}

/**
 * Demonstrates the dangers and solutions
 */
void demonstrateLogoutAudit() {
    std::cout << "========================================\n";
    std::cout << "Logout Audit: Access After Deletion Demo\n";
    std::cout << "========================================\n";
    
    // This will crash or produce undefined behavior
    std::cout << "\nPress Enter to see DANGEROUS example (may crash)...\n";
    std::cin.get();
    
    try {
        dangerousLogoutSequence();
    } catch (...) {
        std::cout << "Program crashed as expected\n";
    }
    
    std::cout << "\nPress Enter for SAFE examples...\n";
    std::cin.get();
    
    // Safe approaches
    safeLogoutSequenceWithCopy();
    safeLogoutSequenceLogFirst();
    safeSmartPointerExample();
    raiiAuditExample();
    
    // Show audit log contents
    std::cout << "\n=== Audit Log Contents ===\n";
    std::ifstream log_file("audit.log");
    if (log_file.is_open()) {
        std::string line;
        while (std::getline(log_file, line)) {
            std::cout << line << "\n";
        }
        log_file.close();
    }
}

/**
 * Production-ready solution with comprehensive safety
 */
class ProductionUserManager {
private:
    struct UserData {
        std::unique_ptr<User> user;
        std::string username_copy;  // For audit after deletion
        int id_copy;
        
        UserData(std::unique_ptr<User> u) 
            : user(std::move(u))
            , username_copy(this->user ? this->user->getUsername() : "")
            , id_copy(this->user ? this->user->getId() : -1) {}
    };
    
    std::vector<UserData> active_users;
    AuditLogger logger;
    
public:
    int createUser(const std::string& name, const std::string& email) {
        static int next_id = 2000;
        int id = next_id++;
        
        auto user = std::make_unique<User>(id, name, email);
        active_users.emplace_back(std::move(user));
        
        logger.log("LOGIN", name, id);
        return id;
    }
    
    bool logoutUser(int user_id) {
        for (auto it = active_users.begin(); it != active_users.end(); ++it) {
            if (it->user && it->user->getId() == user_id) {
                // Log using stored copy (safe even after deletion)
                logger.log("LOGOUT", it->username_copy, it->id_copy);
                
                // Delete the user
                it->user.reset();  // Calls User destructor
                
                // Remove from active users
                active_users.erase(it);
                
                std::cout << "User " << user_id << " logged out and removed\n";
                return true;
            }
        }
        return false;
    }
    
    ~ProductionUserManager() {
        // Log all remaining users on shutdown
        for (const auto& data : active_users) {
            if (data.user) {
                logger.log("SESSION_INTERRUPTED", data.username_copy, data.id_copy);
            }
        }
        std::cout << "User manager shutdown, all users logged\n";
    }
};

int main() {
    try {
        // Demonstrate the issue
        demonstrateLogoutAudit();
        
        // Production example
        std::cout << "\n=== Production User Manager ===\n";
        ProductionUserManager pum;
        
        int user1 = pum.createUser("alice_prod", "alice@company.com");
        int user2 = pum.createUser("bob_prod", "bob@company.com");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        pum.logoutUser(user1);
        pum.logoutUser(user2);
        
        // Summary
        std::cout << "\n=== Key Lessons ===\n";
        std::cout << "1. NEVER access objects after deletion (undefined behavior)\n";
        std::cout << "2. Copy data before deletion if needed after\n";
        std::cout << "3. Log before deletion when possible\n";
        std::cout << "4. Use smart pointers for automatic management\n";
        std::cout << "5. RAII wrappers can handle audit automatically\n";
        std::cout << "6. Store copies of data needed for post-mortem audit\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}