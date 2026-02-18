#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

// Secure memory allocation with automatic clearing
class SecureMemory {
private:
    char* data;
    size_t size;
    
    // Secure memory clear function
    static void secureZero(void* ptr, size_t len) {
        if (ptr == nullptr) return;
        
        // Use volatile to prevent compiler optimization
        volatile char* p = reinterpret_cast<volatile char*>(ptr);
        while (len--) {
            *p++ = 0;
        }
        
        // Additional memory barrier to ensure clearing
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }
    
public:
    explicit SecureMemory(size_t n) : size(n) {
        data = new char[n];
    }
    
    ~SecureMemory() {
        clear();
    }
    
    // Disable copy
    SecureMemory(const SecureMemory&) = delete;
    SecureMemory& operator=(const SecureMemory&) = delete;
    
    // Enable move
    SecureMemory(SecureMemory&& other) noexcept 
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }
    
    void clear() {
        if (data) {
            secureZero(data, size);
            delete[] data;
            data = nullptr;
            size = 0;
        }
    }
    
    char* get() { return data; }
    const char* get() const { return data; }
    size_t getSize() const { return size; }
};

// Secure string class for passwords
class SecureString {
private:
    std::vector<char> buffer;
    
    // Platform-specific secure memory clearing
    static void secureZero(void* ptr, size_t len) {
        if (ptr == nullptr) return;
        
        #if defined(_WIN32) || defined(_WIN64)
            // Windows: SecureZeroMemory is guaranteed not to be optimized
            SecureZeroMemory(ptr, len);
        #else
            // Linux/Unix: Use explicit_bzero or equivalent
            #if defined(__linux__)
                explicit_bzero(ptr, len);
            #elif defined(__APPLE__)
                memset_s(ptr, len, 0, len);
            #else
                // Fallback with volatile
                volatile char* p = reinterpret_cast<volatile char*>(ptr);
                while (len--) {
                    *p++ = 0;
                }
            #endif
        #endif
    }
    
public:
    SecureString() = default;
    
    explicit SecureString(const char* str) {
        if (str) {
            size_t len = strlen(str);
            buffer.resize(len + 1);
            std::copy(str, str + len, buffer.data());
            buffer[len] = '\0';
        }
    }
    
    ~SecureString() {
        clear();
    }
    
    // Disable copy
    SecureString(const SecureString&) = delete;
    SecureString& operator=(const SecureString&) = delete;
    
    // Enable move
    SecureString(SecureString&& other) noexcept 
        : buffer(std::move(other.buffer)) {}
    
    void clear() {
        if (!buffer.empty()) {
            secureZero(buffer.data(), buffer.size());
            buffer.clear();
            buffer.shrink_to_fit();
        }
    }
    
    void assign(const char* str) {
        clear();
        if (str) {
            size_t len = strlen(str);
            buffer.resize(len + 1);
            std::copy(str, str + len, buffer.data());
            buffer[len] = '\0';
        }
    }
    
    const char* c_str() const {
        return buffer.empty() ? nullptr : buffer.data();
    }
    
    bool empty() const {
        return buffer.empty();
    }
};

// Example usage with password input
class PasswordManager {
private:
    SecureString password;
    
    // Hide password input
    static std::string readPassword() {
        std::string pwd;
        char ch;
        
        std::cout << "Enter password: ";
        
        // Simple hidden input (platform-specific implementations better)
        #if defined(_WIN32) || defined(_WIN64)
            // Windows: Use getch()
            #include <conio.h>
            while ((ch = _getch()) != '\r') { // Enter key
                if (ch == '\b' && !pwd.empty()) { // Backspace
                    std::cout << "\b \b";
                    pwd.pop_back();
                } else if (ch != '\b') {
                    pwd.push_back(ch);
                    std::cout << '*';
                }
            }
        #else
            // Unix-like: Use termios
            #include <termios.h>
            #include <unistd.h>
            
            struct termios oldt, newt;
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~ECHO; // Disable echo
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            
            std::getline(std::cin, pwd);
            
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore echo
        #endif
        
        std::cout << std::endl;
        return pwd;
    }
    
public:
    void setPassword() {
        std::string pwd = readPassword();
        password.assign(pwd.c_str());
        
        // Clear the temporary string
        std::fill(pwd.begin(), pwd.end(), 0);
    }
    
    bool verifyPassword(const char* input) {
        if (!password.c_str() || !input) return false;
        
        // Constant-time comparison to prevent timing attacks
        const char* stored = password.c_str();
        size_t len1 = strlen(stored);
        size_t len2 = strlen(input);
        
        if (len1 != len2) return false;
        
        volatile int result = 0;
        for (size_t i = 0; i < len1; ++i) {
            result |= stored[i] ^ input[i];
        }
        
        return result == 0;
    }
    
    ~PasswordManager() {
        password.clear();
    }
};

int main() {
    {
        PasswordManager pm;
        
        std::cout << "Set your password:" << std::endl;
        pm.setPassword();
        
        std::cout << "\nVerify your password:" << std::endl;
        std::string verify = PasswordManager::readPassword(); // Using same hidden input
        
        if (pm.verifyPassword(verify.c_str())) {
            std::cout << "Password verified successfully!" << std::endl;
        } else {
            std::cout << "Password verification failed!" << std::endl;
        }
        
        // Clear verification string
        std::fill(verify.begin(), verify.end(), 0);
    } // PasswordManager destructor automatically clears password
    
    // Memory is securely cleared when objects go out of scope
    
    return 0;
}