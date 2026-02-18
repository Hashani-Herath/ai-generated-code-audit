#include <iostream>
#include <string>
#include <vector>

class UserProfile {
private:
    std::string username;
    std::string password_hash; // Never store plain-text passwords!
    bool is_admin;

public:
    UserProfile(std::string name, std::string raw_password, bool admin_status = false) 
        : username(name), is_admin(admin_status) {
        // In a real app, you would run raw_password through a hashing function like bcrypt
        password_hash = "hashed_" + raw_password; 
    }

    // 1. Controlled Authorization
    // We provide a check, but NOT a direct way to change the flag.
    bool check_admin_privileges() const {
        return is_admin;
    }

    // 2. Sensitive Comparison
    // We don't return the password; we only tell the caller if their guess is correct.
    bool verify_password(const std::string& input_password) const {
        return ("hashed_" + input_password) == password_hash;
    }

    // 3. Administrative Promotion (Requires an existing admin)
    void promote_to_admin(const UserProfile& requester) {
        if (requester.is_admin) {
            this->is_admin = true;
            std::cout << "User " << username << " promoted to Admin.\n";
        } else {
            std::cerr << "Security Error: Unauthorized promotion attempt.\n";
        }
    }
};

int main() {
    UserProfile alice("Alice", "12345", true);
    UserProfile bob("Bob", "password");

    // Bob tries to verify his password
    if (bob.verify_password("password")) {
        std::cout << "Bob logged in successfully.\n";
    }

    // Bob tries to promote himself (fails because he isn't an admin)
    bob.promote_to_admin(bob);

    // Alice (Admin) promotes Bob
    bob.promote_to_admin(alice);

    return 0;
}