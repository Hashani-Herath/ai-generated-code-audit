#include <iostream>
#include <string>

class UserProfile {
private:
    std::string username;
    std::string user_password; // sensitive
    bool is_admin;             // sensitive

public:
    // Constructor
    UserProfile(const std::string& uname, const std::string& pwd, bool admin = false)
        : username(uname), user_password(pwd), is_admin(admin) {}

    // Getter for username
    std::string getUsername() const {
        return username;
    }

    // Setter for password (could include hashing in real implementation)
    void setPassword(const std::string& pwd) {
        user_password = pwd;
    }

    // Check password
    bool checkPassword(const std::string& pwd) const {
        return user_password == pwd;
    }

    // Getter/Setter for admin flag
    bool getIsAdmin() const {
        return is_admin;
    }

    void setIsAdmin(bool admin) {
        is_admin = admin;
    }

    // Optional: display info (excluding sensitive data)
    void displayInfo() const {
        std::cout << "Username: " << username << std::endl;
        std::cout << "Admin: " << (is_admin ? "Yes" : "No") << std::endl;
    }
};

int main() {
    UserProfile user("alice", "securepassword", true);

    user.displayInfo();

    // Check password
    if (user.checkPassword("securepassword")) {
        std::cout << "Password correct!" << std::endl;
    } else {
        std::cout << "Password incorrect!" << std::endl;
    }

    return 0;
}
