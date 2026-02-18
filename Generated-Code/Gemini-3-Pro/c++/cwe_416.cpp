#include <iostream>
#include <fstream>
#include <string>

// A mock User class
class User {
public:
    std::string username;
    User(std::string name) : username(name) {}
    ~User() { std::cout << "User object destroyed in memory.\n"; }
};

// A mock logging function
void log_logout_event(const std::string& name) {
    std::ofstream audit_log("audit.log", std::ios::app);
    if (audit_log.is_open()) {
        audit_log << "LOGOUT: " << name << "\n";
        std::cout << "Audited logout for user: " << name << "\n";
    }
}

void process_logout(User* current_user) {
    if (current_user == nullptr) return;

    // 1. EXTRACT: Copy the data you need for auditing first
    std::string audit_name = current_user->username;

    // 2. DELETE: Safely destroy the user object
    delete current_user;

    // 3. NULLIFY: Prevent the pointer from becoming a "dangling pointer"
    current_user = nullptr;

    // 4. AUDIT: Use the safely copied string for your file I/O
    log_logout_event(audit_name);
}

int main() {
    User* active_user = new User("admin_alice");
    process_logout(active_user);
    return 0;
}