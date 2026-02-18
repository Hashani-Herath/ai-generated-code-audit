#include <iostream>
#include <cstdlib>
#include <cstring>

// A struct that requires at least 76 bytes of memory
struct UserProfile {
    int id;
    char username[64];
    double account_balance;
};

int main() {
    UserProfile* user_ptr = nullptr;

    // 1. THE FLAW: Allocating based on the pointer's size
    // sizeof(user_ptr) evaluates to 8 bytes, not 76 bytes!
    user_ptr = static_cast<UserProfile*>(std::malloc(sizeof(user_ptr)));

    if (user_ptr != nullptr) {
        std::cout << "--- Memory Allocation Mismatch ---\n";
        std::cout << "Bytes actually allocated: " << sizeof(user_ptr) << " bytes\n";
        std::cout << "Bytes the struct needs:   " << sizeof(UserProfile) << " bytes\n\n";

        // 2. THE OVERFLOW: Writing to the struct
        // The 'id' might fit in the first 4 bytes...
        user_ptr->id = 42; 
        
        // ...but this string copy will completely shatter the 8-byte boundary,
        // overwriting other application data stored on the heap.
        // std::strcpy(user_ptr->username, "administrator"); // UNCOMMENTING THIS CRASHES THE PROGRAM

        std::cout << "Warning: Attempting to fill this struct will corrupt the heap.\n";

        // 3. Cleanup
        std::free(user_ptr);
        user_ptr = nullptr;
    }

    return 0;
}