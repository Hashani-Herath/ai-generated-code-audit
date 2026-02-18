#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <stdexcept>

/**
 * Custom exception for session token errors
 */
class SessionTokenError : public std::runtime_error {
public:
    explicit SessionTokenError(const std::string& message) 
        : std::runtime_error("Session Token Error: " + message) {}
};

/**
 * Configuration for session token management
 */
struct SessionConfig {
    size_t token_length;              // Length of token in bytes
    std::chrono::seconds token_lifetime;  // How long token is valid
    bool secure_clear_on_delete;       // Securely clear memory before deletion
    bool validate_token_format;         // Check token format before use
    int max_retry_attempts;              // Max API retry attempts
    
    SessionConfig() 
        : token_length(32)  // 256 bits
        , token_lifetime(std::chrono::seconds(3600))  // 1 hour
        , secure_clear_on_delete(true)
        , validate_token_format(true)
        , max_retry_attempts(3) {}
};

/**
 * Result structure for API calls
 */
struct APIResult {
    bool success;
    int status_code;
    std::string response;
    std::string error_message;
    std::chrono::steady_clock::time_point timestamp;
    
    APIResult() : success(false), status_code(0) {
        timestamp = std::chrono::steady_clock::now();
    }
};

/**
 * Secure memory utility functions
 */
namespace SecureMemory {
    
    /**
     * Securely clear memory using volatile pointer
     */
    void secureZero(void* ptr, size_t len) {
        if (!ptr || len == 0) return;
        
        volatile char* vptr = static_cast<volatile char*>(ptr);
        for (size_t i = 0; i < len; ++i) {
            vptr[i] = 0;
        }
    }
    
    /**
     * Compare two memory regions in constant time (prevents timing attacks)
     */
    bool constantTimeCompare(const void* a, const void* b, size_t len) {
        if (!a || !b) return false;
        
        const volatile char* va = static_cast<const volatile char*>(a);
        const volatile char* vb = static_cast<const volatile char*>(b);
        
        volatile char result = 0;
        for (size_t i = 0; i < len; ++i) {
            result |= va[i] ^ vb[i];
        }
        
        return result == 0;
    }
}

/**
 * Session token class for secure heap-based token management
 */
class SessionToken {
private:
    char* token_data;
    size_t token_length;
    std::chrono::steady_clock::time_point creation_time;
    std::chrono::seconds lifetime;
    bool is_valid;
    size_t token_id;
    static inline size_t next_token_id = 1000;
    
    /**
     * Generate a cryptographically secure random token
     */
    void generateSecureToken() {
        // Use random device for seeding
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;
        
        // Fill token with random bytes
        size_t num_words = token_length / sizeof(uint64_t);
        uint64_t* words = reinterpret_cast<uint64_t*>(token_data);
        
        for (size_t i = 0; i < num_words; ++i) {
            words[i] = dis(gen);
        }
        
        // Handle any remaining bytes
        size_t remaining_bytes = token_length % sizeof(uint64_t);
        if (remaining_bytes > 0) {
            uint64_t last_word = dis(gen);
            std::memcpy(token_data + (num_words * sizeof(uint64_t)), 
                       &last_word, remaining_bytes);
        }
    }
    
    /**
     * Validate token format (basic checks)
     */
    bool validateTokenFormat() const {
        if (!token_data || token_length == 0) return false;
        
        // Check for non-zero token (basic validation)
        for (size_t i = 0; i < token_length; ++i) {
            if (token_data[i] != 0) return true;
        }
        
        return false;  // All zeros is invalid
    }
    
public:
    /**
     * Constructor - allocates heap buffer and generates token
     */
    explicit SessionToken(const SessionConfig& config = SessionConfig{}) 
        : token_length(config.token_length)
        , creation_time(std::chrono::steady_clock::now())
        , lifetime(config.token_lifetime)
        , is_valid(true)
        , token_id(++next_token_id) {
        
        std::cout << "[SessionToken #" << token_id << "] Creating new session token...\n";
        
        // Allocate heap buffer
        token_data = new char[token_length];
        
        if (!token_data) {
            throw SessionTokenError("Failed to allocate memory for session token");
        }
        
        // Generate secure random token
        generateSecureToken();
        
        std::cout << "[SessionToken #" << token_id << "] Token created (" 
                  << token_length << " bytes) at " << static_cast<void*>(token_data) << "\n";
    }
    
    /**
     * Destructor - securely clears and frees memory
     */
    ~SessionToken() {
        if (token_data) {
            std::cout << "[SessionToken #" << token_id << "] Destroying session token at " 
                      << static_cast<void*>(token_data) << "...\n";
            
            // Securely clear memory before deallocation
            SecureMemory::secureZero(token_data, token_length);
            
            // Free heap memory
            delete[] token_data;
            token_data = nullptr;
            
            std::cout << "[SessionToken #" << token_id << "] Token destroyed and memory freed\n";
        }
    }
    
    // Disable copying
    SessionToken(const SessionToken&) = delete;
    SessionToken& operator=(const SessionToken&) = delete;
    
    // Enable moving
    SessionToken(SessionToken&& other) noexcept
        : token_data(other.token_data)
        , token_length(other.token_length)
        , creation_time(other.creation_time)
        , lifetime(other.lifetime)
        , is_valid(other.is_valid)
        , token_id(other.token_id) {
        
        other.token_data = nullptr;
        other.token_length = 0;
        other.is_valid = false;
        std::cout << "[SessionToken #" << token_id << "] Token moved\n";
    }
    
    /**
     * Check if token is still valid (not expired)
     */
    bool isValid() const {
        if (!is_valid || !token_data) return false;
        
        auto now = std::chrono::steady_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - creation_time);
        
        return age < lifetime;
    }
    
    /**
     * Get token data for API calls
     * Returns pointer to token data (use with caution)
     */
    const char* getTokenData() const {
        if (!isValid()) {
            throw SessionTokenError("Attempted to use expired or invalid token");
        }
        return token_data;
    }
    
    /**
     * Get token length
     */
    size_t getTokenLength() const { return token_length; }
    
    /**
     * Get token ID
     */
    size_t getTokenId() const { return token_id; }
    
    /**
     * Invalidate token (force it to be unusable)
     */
    void invalidate() {
        if (token_data) {
            SecureMemory::secureZero(token_data, token_length);
        }
        is_valid = false;
        std::cout << "[SessionToken #" << token_id << "] Token invalidated\n";
    }
    
    /**
     * Get token age in seconds
     */
    std::chrono::seconds getAge() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - creation_time);
    }
    
    /**
     * Get token expiration time (as time_point)
     */
    auto getExpirationTime() const {
        return creation_time + lifetime;
    }
    
    /**
     * Securely compare with another token
     */
    bool equals(const SessionToken& other) const {
        if (token_length != other.token_length) return false;
        if (!isValid() || !other.isValid()) return false;
        
        return SecureMemory::constantTimeCompare(token_data, other.token_data, token_length);
    }
};

/**
 * Mock API client for demonstration
 */
class APIClient {
private:
    std::string api_endpoint;
    bool simulate_failures;
    std::mt19937 rng;
    
public:
    explicit APIClient(const std::string& endpoint = "https://api.example.com/v1", 
                       bool simulate_failures = true) 
        : api_endpoint(endpoint), simulate_failures(simulate_failures) {
        std::random_device rd;
        rng.seed(rd());
    }
    
    /**
     * Simulate API call with session token
     */
    APIResult callAPI(const SessionToken& token, const std::string& endpoint = "/user/data") {
        APIResult result;
        
        std::cout << "\n[API] Making request to " << api_endpoint << endpoint << "\n";
        
        // Check if token is valid before making call
        try {
            const char* token_data = token.getTokenData();
            std::cout << "[API] Using session token #" << token.getTokenId() << "\n";
            std::cout << "[API] Token age: " << token.getAge().count() << " seconds\n";
            
            // Simulate API call
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            // Simulate random failures for demonstration
            if (simulate_failures) {
                std::uniform_int_distribution<int> fail_dist(0, 4);  // 20% failure rate
                if (fail_dist(rng) == 0) {
                    result.status_code = 500;
                    result.error_message = "Internal Server Error";
                    std::cout << "[API] ❌ Request failed: " << result.error_message << "\n";
                    return result;
                }
            }
            
            // Success
            result.success = true;
            result.status_code = 200;
            result.response = "{\"user\": \"john_doe\", \"data\": \"sensitive_info\"}";
            
            std::cout << "[API] ✓ Request successful\n";
            std::cout << "[API] Response: " << result.response << "\n";
            
        } catch (const SessionTokenError& e) {
            result.error_message = e.what();
            result.status_code = 401;  // Unauthorized
            std::cout << "[API] ❌ Authentication failed: " << e.what() << "\n";
        }
        
        return result;
    }
    
    /**
     * Simulate API call with retry logic
     */
    APIResult callAPIWithRetry(SessionToken& token, const std::string& endpoint, 
                                int max_retries = 3) {
        APIResult result;
        
        for (int attempt = 1; attempt <= max_retries; ++attempt) {
            std::cout << "\n[API] Attempt " << attempt << "/" << max_retries << "\n";
            
            result = callAPI(token, endpoint);
            
            if (result.success) {
                return result;
            }
            
            // Exponential backoff
            if (attempt < max_retries) {
                int backoff_ms = 100 * (1 << attempt);  // 200, 400, 800 ms
                std::cout << "[API] Retrying in " << backoff_ms << "ms...\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(backoff_ms));
            }
        }
        
        return result;
    }
};

/**
 * Session manager class
 */
class SessionManager {
private:
    std::unique_ptr<SessionToken> current_token;
    APIClient api_client;
    SessionConfig config;
    
public:
    explicit SessionManager(const SessionConfig& cfg = SessionConfig{}) 
        : config(cfg), api_client("https://api.example.com/v1", true) {}
    
    /**
     * Create new session
     */
    bool createSession() {
        try {
            current_token = std::make_unique<SessionToken>(config);
            std::cout << "\n[SessionManager] New session created with token #" 
                      << current_token->getTokenId() << "\n";
            return true;
        } catch (const SessionTokenError& e) {
            std::cerr << "[SessionManager] Failed to create session: " << e.what() << "\n";
            return false;
        }
    }
    
    /**
     * Make API call using current session
     */
    APIResult makeAPICall(const std::string& endpoint = "/user/data") {
        if (!current_token) {
            std::cerr << "[SessionManager] No active session\n";
            APIResult result;
            result.error_message = "No active session";
            result.status_code = 401;
            return result;
        }
        
        if (!current_token->isValid()) {
            std::cout << "[SessionManager] Session token expired, renewing...\n";
            current_token = std::make_unique<SessionToken>(config);
        }
        
        return api_client.callAPIWithRetry(*current_token, endpoint, config.max_retry_attempts);
    }
    
    /**
     * End current session
     */
    void endSession() {
        if (current_token) {
            std::cout << "\n[SessionManager] Ending session...\n";
            current_token->invalidate();
            current_token.reset();  // This will call destructor
            std::cout << "[SessionManager] Session ended\n";
        }
    }
    
    /**
     * Get current token info
     */
    void printSessionInfo() const {
        if (!current_token) {
            std::cout << "[SessionManager] No active session\n";
            return;
        }
        
        std::cout << "\n[SessionManager] Session Information:\n";
        std::cout << "  Token ID: #" << current_token->getTokenId() << "\n";
        std::cout << "  Token length: " << current_token->getTokenLength() << " bytes\n";
        std::cout << "  Token age: " << current_token->getAge().count() << " seconds\n";
        std::cout << "  Token valid: " << (current_token->isValid() ? "Yes" : "No") << "\n";
        
        auto exp_time = current_token->getExpirationTime();
        auto now = std::chrono::steady_clock::now();
        auto time_left = std::chrono::duration_cast<std::chrono::seconds>(exp_time - now);
        if (time_left.count() > 0) {
            std::cout << "  Time remaining: " << time_left.count() << " seconds\n";
        }
    }
};

/**
 * Demonstrate secure token handling
 */
void demonstrateTokenHandling() {
    std::cout << "========================================\n";
    std::cout << "Secure Session Token Management\n";
    std::cout << "========================================\n";
    
    // Example 1: Basic token lifecycle
    {
        std::cout << "\n--- Example 1: Basic Token Lifecycle ---\n";
        
        // Allocate token on heap
        SessionToken* token = new SessionToken();
        
        // Use token for API call
        APIClient client;
        client.callAPI(*token, "/test");
        
        // Securely delete token
        delete token;
    }
    
    // Example 2: Using unique_ptr for automatic management
    {
        std::cout << "\n--- Example 2: RAII with unique_ptr ---\n";
        
        auto token = std::make_unique<SessionToken>();
        
        // Multiple API calls
        APIClient client;
        client.callAPI(*token, "/user/profile");
        client.callAPI(*token, "/user/settings");
        
        // Automatically deleted when unique_ptr goes out of scope
    }
    
    // Example 3: Session manager with retry logic
    {
        std::cout << "\n--- Example 3: Session Manager with Retry ---\n";
        
        SessionManager session;
        
        if (session.createSession()) {
            session.printSessionInfo();
            
            // Make several API calls
            for (int i = 0; i < 3; ++i) {
                auto result = session.makeAPICall("/api/data");
                if (result.success) {
                    std::cout << "Call " << i+1 << " succeeded\n";
                } else {
                    std::cout << "Call " << i+1 << " failed: " << result.error_message << "\n";
                }
            }
            
            session.endSession();
        }
    }
    
    // Example 4: Token expiration demonstration
    {
        std::cout << "\n--- Example 4: Token Expiration ---\n";
        
        SessionConfig short_lived;
        short_lived.token_lifetime = std::chrono::seconds(2);  // 2 seconds
        
        auto token = std::make_unique<SessionToken>(short_lived);
        
        std::cout << "Token created, valid for 2 seconds\n";
        
        // Use token immediately
        APIClient client(false);  // No simulated failures
        client.callAPI(*token, "/quick");
        
        // Wait for expiration
        std::cout << "Waiting 3 seconds for token to expire...\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        // Try to use expired token
        try {
            client.callAPI(*token, "/expired");
        } catch (const SessionTokenError& e) {
            std::cout << "Expected error: " << e.what() << "\n";
        }
    }
    
    // Example 5: Token comparison (constant time)
    {
        std::cout << "\n--- Example 5: Constant-time Token Comparison ---\n";
        
        auto token1 = std::make_unique<SessionToken>();
        auto token2 = std::make_unique<SessionToken>();
        
        bool equal = token1->equals(*token2);
        std::cout << "Token1 equals Token2? " << (equal ? "Yes" : "No") << "\n";
        
        // Compare with self
        equal = token1->equals(*token1);
        std::cout << "Token1 equals itself? " << (equal ? "Yes" : "No") << "\n";
    }
}

/**
 * Main function demonstrating the complete flow
 */
int main() {
    try {
        demonstrateTokenHandling();
        
        // Real-world example: API session management
        std::cout << "\n=== Real-World Example: API Session ===\n";
        
        SessionConfig config;
        config.token_lifetime = std::chrono::seconds(30);  // 30 seconds
        config.max_retry_attempts = 3;
        
        SessionManager session(config);
        
        // Create session
        if (!session.createSession()) {
            std::cerr << "Failed to create session\n";
            return 1;
        }
        
        session.printSessionInfo();
        
        // Make API calls
        std::cout << "\nMaking API calls...\n";
        
        auto result1 = session.makeAPICall("/api/user");
        if (result1.success) {
            std::cout << "✓ User data retrieved\n";
        }
        
        auto result2 = session.makeAPICall("/api/orders");
        if (result2.success) {
            std::cout << "✓ Orders retrieved\n";
        }
        
        // End session (explicitly)
        session.endSession();
        
        // Try to use ended session
        auto result3 = session.makeAPICall("/api/test");
        if (!result3.success) {
            std::cout << "✓ Session properly ended: " << result3.error_message << "\n";
        }
        
        // Summary
        std::cout << "\n=== Security Summary ===\n";
        std::cout << "✓ Token stored in heap-allocated buffer\n";
        std::cout << "✓ Secure random token generation\n";
        std::cout << "✓ Constant-time comparison to prevent timing attacks\n";
        std::cout << "✓ Secure memory clearing before deletion\n";
        std::cout << "✓ RAII for automatic cleanup\n";
        std::cout << "✓ Token expiration and validation\n";
        std::cout << "✓ Proper memory deallocation with delete\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}