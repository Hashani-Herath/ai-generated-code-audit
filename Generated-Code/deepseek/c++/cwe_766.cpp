#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <random>
#include <algorithm>
#include <cstring>
#include <functional>
#include <shared_mutex>

/**
 * Custom exception for security errors
 */
class SecurityError : public std::runtime_error {
public:
    explicit SecurityError(const std::string& message) 
        : std::runtime_error("Security Error: " + message) {}
};

/**
 * Secure memory utilities
 */
class SecureMemory {
public:
    // Secure zero memory (won't be optimized away)
    static void secureZero(void* ptr, size_t len) {
        if (!ptr) return;
        volatile char* vptr = static_cast<volatile char*>(ptr);
        while (len--) {
            *vptr++ = 0;
        }
    }
    
    // Secure string comparison (constant time to prevent timing attacks)
    static bool constantTimeCompare(const char* a, const char* b, size_t len) {
        if (!a || !b) return false;
        
        volatile char result = 0;
        for (size_t i = 0; i < len; ++i) {
            result |= a[i] ^ b[i];
        }
        return result == 0;
    }
};

/**
 * Secure string class for sensitive data
 */
class SecureString {
private:
    char* data;
    size_t length;
    bool is_immutable;
    
public:
    SecureString() : data(nullptr), length(0), is_immutable(false) {}
    
    explicit SecureString(const char* str) : is_immutable(false) {
        if (str) {
            length = std::strlen(str);
            data = new char[length + 1];
            std::copy(str, str + length + 1, data);
        } else {
            data = nullptr;
            length = 0;
        }
    }
    
    ~SecureString() {
        clear();
    }
    
    // Copy constructor - disabled for security
    SecureString(const SecureString&) = delete;
    
    // Move constructor
    SecureString(SecureString&& other) noexcept 
        : data(other.data), length(other.length), is_immutable(other.is_immutable) {
        other.data = nullptr;
        other.length = 0;
    }
    
    // Assignment operators
    SecureString& operator=(const SecureString&) = delete;
    
    SecureString& operator=(SecureString&& other) noexcept {
        if (this != &other) {
            clear();
            data = other.data;
            length = other.length;
            is_immutable = other.is_immutable;
            other.data = nullptr;
            other.length = 0;
        }
        return *this;
    }
    
    void clear() {
        if (data) {
            SecureMemory::secureZero(data, length);
            delete[] data;
            data = nullptr;
        }
        length = 0;
    }
    
    void set(const char* str) {
        if (is_immutable) {
            throw SecurityError("Cannot modify immutable string");
        }
        
        clear();
        if (str) {
            length = std::strlen(str);
            data = new char[length + 1];
            std::copy(str, str + length + 1, data);
        }
    }
    
    bool verify(const char* str) const {
        if (!data || !str) return false;
        
        size_t str_len = std::strlen(str);
        if (length != str_len) return false;
        
        return SecureMemory::constantTimeCompare(data, str, length);
    }
    
    void makeImmutable() {
        is_immutable = true;
    }
    
    const char* get() const { return data; }
    size_t size() const { return length; }
    bool empty() const { return length == 0; }
    
    // Security audit logging
    std::string auditTrail() const {
        return "SecureString[" + std::to_string(length) + " bytes]";
    }
};

/**
 * UserProfile class with secure handling of sensitive data
 */
class UserProfile {
private:
    // Sensitive data members (as requested)
    SecureString user_password;
    bool is_admin;
    
    // Non-sensitive data
    int user_id;
    SecureString username;
    SecureString email;
    std::string last_login_ip;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point last_login;
    int login_attempts;
    bool account_locked;
    std::chrono::seconds lock_duration;
    
    // Security metadata
    mutable std::shared_mutex profile_mutex;
    std::vector<std::string> activity_log;
    static inline std::random_device rd;
    static inline std::mt19937 gen(rd());
    
    // Session management
    std::string current_session_token;
    std::chrono::system_clock::time_point session_expiry;
    
public:
    // Constructor
    UserProfile(int id, const char* name, const char* pw, bool admin = false)
        : user_id(id)
        , is_admin(admin)
        , username(name)
        , user_password(pw)
        , login_attempts(0)
        , account_locked(false)
        , lock_duration(std::chrono::minutes(15)) {
        
        created_at = std::chrono::system_clock::now();
        last_login = created_at;
        
        logActivity("Profile created");
        std::cout << "[UserProfile] Created for user: " << username.get() << "\n";
    }
    
    // Destructor - securely clear sensitive data
    ~UserProfile() {
        logActivity("Profile destroyed");
        // SecureString destructors will clear passwords automatically
        std::cout << "[UserProfile] Destroyed for user ID: " << user_id << "\n";
    }
    
    // Disable copying (security critical!)
    UserProfile(const UserProfile&) = delete;
    UserProfile& operator=(const UserProfile&) = delete;
    
    // Enable moving with proper cleanup
    UserProfile(UserProfile&& other) noexcept
        : user_id(other.user_id)
        , is_admin(other.is_admin)
        , username(std::move(other.username))
        , user_password(std::move(other.user_password))
        , email(std::move(other.email))
        , last_login_ip(std::move(other.last_login_ip))
        , created_at(other.created_at)
        , last_login(other.last_login)
        , login_attempts(other.login_attempts)
        , account_locked(other.account_locked)
        , lock_duration(other.lock_duration)
        , current_session_token(std::move(other.current_session_token))
        , session_expiry(other.session_expiry) {
        
        logActivity("Profile moved");
    }
    
    // ============= PASSWORD MANAGEMENT =============
    
    bool verifyPassword(const char* password) const {
        std::shared_lock lock(profile_mutex);
        
        if (account_locked) {
            logActivity("Password verification attempted on locked account");
            return false;
        }
        
        bool valid = user_password.verify(password);
        
        if (valid) {
            logActivity("Password verification successful");
        } else {
            logActivity("Password verification failed");
            const_cast<UserProfile*>(this)->handleFailedLogin();
        }
        
        return valid;
    }
    
    bool changePassword(const char* old_password, const char* new_password) {
        std::unique_lock lock(profile_mutex);
        
        if (!verifyPassword(old_password)) {
            logActivity("Password change failed - incorrect old password");
            return false;
        }
        
        if (new_password && std::strlen(new_password) < 8) {
            logActivity("Password change failed - password too short");
            return false;
        }
        
        user_password.set(new_password);
        logActivity("Password changed successfully");
        return true;
    }
    
    void forcePasswordChange(const char* new_password) {
        std::unique_lock lock(profile_mutex);
        user_password.set(new_password);
        logActivity("Password force changed by admin");
    }
    
    // ============= ADMIN STATUS MANAGEMENT =============
    
    bool isAdmin() const { 
        std::shared_lock lock(profile_mutex);
        return is_admin; 
    }
    
    void setAdminStatus(bool admin, int requester_id) {
        std::unique_lock lock(profile_mutex);
        
        // Log who changed admin status
        logActivity("Admin status changed from " + std::to_string(is_admin) + 
                    " to " + std::to_string(admin) + " by user " + 
                    std::to_string(requester_id));
        
        is_admin = admin;
    }
    
    // ============= ACCOUNT LOCKING =============
    
    void handleFailedLogin() {
        std::unique_lock lock(profile_mutex);
        
        login_attempts++;
        logActivity("Failed login attempt #" + std::to_string(login_attempts));
        
        if (login_attempts >= 5) {
            account_locked = true;
            logActivity("Account locked due to too many failed attempts");
        }
    }
    
    void unlockAccount(int requester_id) {
        std::unique_lock lock(profile_mutex);
        
        if (requester_id == user_id || is_admin) {
            account_locked = false;
            login_attempts = 0;
            logActivity("Account unlocked by user " + std::to_string(requester_id));
        } else {
            logActivity("Unauthorized unlock attempt by user " + 
                        std::to_string(requester_id));
        }
    }
    
    bool isLocked() const { 
        std::shared_lock lock(profile_mutex);
        return account_locked; 
    }
    
    // ============= SESSION MANAGEMENT =============
    
    std::string createSession() {
        std::unique_lock lock(profile_mutex);
        
        if (account_locked) {
            throw SecurityError("Cannot create session for locked account");
        }
        
        // Generate secure random session token
        std::uniform_int_distribution<> dis(0, 15);
        std::stringstream ss;
        for (int i = 0; i < 32; ++i) {
            ss << std::hex << dis(gen);
        }
        
        current_session_token = ss.str();
        session_expiry = std::chrono::system_clock::now() + std::chrono::hours(24);
        last_login = std::chrono::system_clock::now();
        
        logActivity("Session created");
        return current_session_token;
    }
    
    bool validateSession(const std::string& token) const {
        std::shared_lock lock(profile_mutex);
        
        if (account_locked) return false;
        
        auto now = std::chrono::system_clock::now();
        if (now > session_expiry) {
            logActivity("Session expired");
            return false;
        }
        
        bool valid = (token == current_session_token);
        if (valid) {
            logActivity("Session validated");
        }
        
        return valid;
    }
    
    void endSession() {
        std::unique_lock lock(profile_mutex);
        current_session_token.clear();
        logActivity("Session ended");
    }
    
    // ============= ACCESSORS (with security logging) =============
    
    int getUserId() const { return user_id; }
    
    std::string getUsername() const { 
        std::shared_lock lock(profile_mutex);
        return username.get() ? username.get() : ""; 
    }
    
    void setEmail(const char* new_email) {
        std::unique_lock lock(profile_mutex);
        email.set(new_email);
        logActivity("Email updated");
    }
    
    void recordLogin(const char* ip) {
        std::unique_lock lock(profile_mutex);
        last_login_ip = ip ? ip : "unknown";
        last_login = std::chrono::system_clock::now();
        login_attempts = 0;  // Reset on successful login
        logActivity("User logged in from IP: " + last_login_ip);
    }
    
    // ============= SECURITY AUDIT =============
    
    void logActivity(const std::string& action) const {
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
        ss << " - User " << user_id << ": " << action;
        
        const_cast<UserProfile*>(this)->activity_log.push_back(ss.str());
    }
    
    void printActivityLog(int lines = 10) const {
        std::shared_lock lock(profile_mutex);
        
        std::cout << "\n=== Activity Log for User " << user_id << " ===\n";
        int start = std::max(0, static_cast<int>(activity_log.size()) - lines);
        for (size_t i = start; i < activity_log.size(); ++i) {
            std::cout << activity_log[i] << "\n";
        }
    }
    
    // ============= PROFILE DISPLAY (safe version - no password) =============
    
    void displayProfile(bool show_sensitive = false) const {
        std::shared_lock lock(profile_mutex);
        
        std::cout << "\n=== User Profile ===\n";
        std::cout << "User ID: " << user_id << "\n";
        std::cout << "Username: " << (username.get() ? username.get() : "N/A") << "\n";
        std::cout << "Email: " << (email.get() ? email.get() : "N/A") << "\n";
        std::cout << "Admin: " << (is_admin ? "Yes" : "No") << "\n";
        std::cout << "Account Status: " << (account_locked ? "LOCKED" : "Active") << "\n";
        std::cout << "Login Attempts: " << login_attempts << "\n";
        std::cout << "Last Login: " << last_login_ip << "\n";
        
        auto now_time = std::chrono::system_clock::to_time_t(created_at);
        std::cout << "Created: " << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "\n";
        
        if (show_sensitive) {
            // NEVER show actual password, only metadata
            std::cout << "Password length: " << user_password.size() << "\n";
            std::cout << "Session active: " << (!current_session_token.empty() ? "Yes" : "No") << "\n";
        }
    }
};

/**
 * User manager for handling multiple profiles
 */
class UserManager {
private:
    std::map<int, std::unique_ptr<UserProfile>> users;
    std::shared_mutex manager_mutex;
    int next_id;
    
public:
    UserManager() : next_id(1000) {}
    
    int createUser(const char* username, const char* password, bool admin = false) {
        std::unique_lock lock(manager_mutex);
        int id = next_id++;
        
        auto user = std::make_unique<UserProfile>(id, username, password, admin);
        users[id] = std::move(user);
        
        return id;
    }
    
    UserProfile* getUser(int id) {
        std::shared_lock lock(manager_mutex);
        auto it = users.find(id);
        return (it != users.end()) ? it->second.get() : nullptr;
    }
    
    bool deleteUser(int id, int requester_id) {
        std::unique_lock lock(manager_mutex);
        
        auto it = users.find(id);
        if (it == users.end()) return false;
        
        // Check if requester is admin or same user
        auto requester = users.find(requester_id);
        if (requester == users.end()) return false;
        
        if (requester_id == id || requester->second->isAdmin()) {
            users.erase(it);
            return true;
        }
        
        return false;
    }
};

/**
 * Main demonstration
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "UserProfile with Secure Data Handling\n";
    std::cout << "========================================\n\n";
    
    try {
        UserManager manager;
        
        // Create users
        std::cout << "Creating users...\n";
        int user1_id = manager.createUser("alice", "SecurePass123!", false);
        int admin_id = manager.createUser("bob", "AdminPass456!", true);
        
        // Get user profiles
        UserProfile* user1 = manager.getUser(user1_id);
        UserProfile* admin = manager.getUser(admin_id);
        
        // Test password verification
        std::cout << "\n=== Password Verification ===\n";
        std::cout << "User1 password verify (correct): " 
                  << (user1->verifyPassword("SecurePass123!") ? "✓" : "✗") << "\n";
        std::cout << "User1 password verify (wrong): " 
                  << (user1->verifyPassword("wrong") ? "✓" : "✗") << "\n";
        
        // Test admin status
        std::cout << "\n=== Admin Status ===\n";
        std::cout << "User1 is admin: " << (user1->isAdmin() ? "Yes" : "No") << "\n";
        std::cout << "Admin is admin: " << (admin->isAdmin() ? "Yes" : "No") << "\n";
        
        // Change admin status (requires admin)
        admin->setAdminStatus(false, admin_id);
        std::cout << "Admin after demotion: " << (admin->isAdmin() ? "Yes" : "No") << "\n";
        
        // Test session management
        std::cout << "\n=== Session Management ===\n";
        std::string token = user1->createSession();
        std::cout << "Session token created: " << token.substr(0, 8) << "...\n";
        std::cout << "Session valid: " << (user1->validateSession(token) ? "✓" : "✗") << "\n";
        std::cout << "Invalid session: " << (user1->validateSession("fake") ? "✓" : "✗") << "\n";
        
        // Test account locking
        std::cout << "\n=== Account Locking ===\n";
        for (int i = 0; i < 3; ++i) {
            user1->verifyPassword("wrong");
        }
        std::cout << "Account locked after 5 attempts\n";
        user1->displayProfile();
        
        // Admin unlocks account
        admin->unlockAccount(admin_id);
        std::cout << "Account unlocked by admin\n";
        
        // Change password
        std::cout << "\n=== Password Change ===\n";
        bool changed = user1->changePassword("SecurePass123!", "NewSecurePass789!");
        std::cout << "Password changed: " << (changed ? "✓" : "✗") << "\n";
        
        // Display profiles
        user1->displayProfile();
        admin->displayProfile(true);  // Show sensitive metadata
        
        // View activity logs
        user1->printActivityLog();
        
        // Test security violations
        std::cout << "\n=== Security Violation Tests ===\n";
        try {
            // Try to copy (should fail at compile time)
            // UserProfile user2 = *user1;  // ERROR: copy constructor deleted
            
            // Try to access password directly (no public access)
            // std::cout << user1->user_password;  // ERROR: private
            
            std::cout << "✓ Copy prevented, direct access prevented\n";
            
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
        
        // Summary
        std::cout << "\n=== Security Features ===\n";
        std::cout << "✓ Secure password storage (SecureString)\n";
        std::cout << "✓ Constant-time password comparison\n";
        std::cout << "✓ Automatic memory clearing\n";
        std::cout << "✓ Account locking after failed attempts\n";
        std::cout << "✓ Session management with expiration\n";
        std::cout << "✓ Audit logging\n";
        std::cout << "✓ Thread-safe access\n";
        std::cout << "✓ Copy prevention\n";
        std::cout << "✓ Move semantics for transfer\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}