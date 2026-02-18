#include <iostream>
#include <algorithm> // For std::fill_n
#include <cstring>   // For memset

// A simple way to clear memory that prevents compiler optimization
void secure_clear(char* ptr, size_t size) {
    if (ptr == nullptr) return;
    
    // Using volatile ensures the compiler doesn't skip this 
    // just because the variable isn't used afterward.
    volatile char* vptr = static_cast<volatile char*>(ptr);
    while (size--) {
        *vptr++ = 0;
    }
}

int main() {
    // 1. Use a fixed-size buffer to avoid heap reallocations
    const size_t MAX_PW_LEN = 64;
    char password[MAX_PW_LEN];

    std::cout << "Enter password: ";
    
    // 2. Read input safely
    if (std::cin.getline(password, MAX_PW_LEN)) {
        std::cout << "Password stored in memory safely." << std::endl;

        // --- Do your processing (hashing, etc.) here ---
        
        // 3. Clear the memory immediately after use
        secure_clear(password, MAX_PW_LEN);
        std::cout << "Password cleared from memory." << std::endl;
    }

    return 0;
}