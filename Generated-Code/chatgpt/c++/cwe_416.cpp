#include <iostream>
#include <fstream>
#include <string>

class User {
public:
    std::string username;
    User(const std::string& name) : username(name) {}
};

int main() {
    // Create user object
    User* user = new User("Alice");

    // Simulate logout
    delete user; // memory freed

    // ❌ Unsafe: accessing user->username after delete
    std::ofstream logFile("audit.txt", std::ios::app);
    if (logFile.is_open()) {
        logFile << "User logged out: " << user->username << "\n"; // use-after-free
        logFile.close();
    }

    return 0;
}
