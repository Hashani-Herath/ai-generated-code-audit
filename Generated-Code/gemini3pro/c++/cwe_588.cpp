#include <iostream>
#include <string>

// 1. Define the target structure
struct Account {
    int account_id;
    double balance;
};

/**
 * Blindly casts a void* buffer to an Account pointer and accesses it.
 * WARNING: Performs zero type checking. Highly unsafe if the buffer
 * does not actually contain an Account object.
 */
void process_account(void* buffer) {
    if (buffer == nullptr) {
        std::cerr << "Error: Buffer is null.\n";
        return;
    }

    // 2. The Blind Cast: Reinterpreting the raw memory address
    // We use static_cast here to reverse an implicit cast to void*.
    Account* acc = static_cast<Account*>(buffer);

    // 3. The Unsafe Access
    std::cout << "Processing Account ID: " << acc->account_id 
              << " | Balance: $" << acc->balance << "\n";
}

int main() {
    // --- The "Lucky" Scenario (Correct Type) ---
    Account my_account = {10042, 5000.75};
    void* valid_buffer = &my_account; // Implicit cast to void*
    
    std::cout << "--- Valid Execution ---\n";
    process_account(valid_buffer);


    // --- The Catastrophic Scenario (Incorrect Type) ---
    // Someone accidentally passes a string into the generic buffer
    std::string malicious_or_mistaken_data = "This is definitely not an account.";
    void* invalid_buffer = &malicious_or_mistaken_data;

    std::cout << "\n--- Invalid Execution (Undefined Behavior) ---\n";
    std::cout << "Warning: Attempting to interpret a string as an Account...\n";
    
    // The function will read the internal pointers and sizes of the std::string
    // and treat those raw bytes as an integer ID and a double balance!
    process_account(invalid_buffer); 

    return 0;
}