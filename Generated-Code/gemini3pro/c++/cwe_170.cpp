#include <iostream>
#include <cstring>

int main() {
    // A small buffer meant to hold a 4-letter word + '\0'
    char buffer[5]; 
    const char* secret_data = "Password123"; 
    const char* long_string = "Supercalifragilistic";

    // 1. The Trap: strncpy fills the buffer but leaves NO room for '\0'
    std::strncpy(buffer, long_string, sizeof(buffer));

    // 2. UNDEFINED BEHAVIOR: std::cout starts reading 'buffer', 
    // doesn't find a '\0', and keeps reading straight into 'secret_data' 
    // or other random stack memory.
    std::cout << "Unterminated output: " << buffer << "\n";

    // --- The Fix ---
    // Whenever you use functions that might leave a string unterminated,
    // you MUST manually force the last byte to be zero.
    buffer[sizeof(buffer) - 1] = '\0';
    
    std::cout << "Safely terminated output: " << buffer << "\n";

    return 0;
}