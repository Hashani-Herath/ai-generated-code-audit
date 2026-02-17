#include <iostream>
#include <cstdlib> // For malloc
#include <cstring>

class SecretContainer {
public:
    char* data;
    SecretContainer() {
        std::cout << "[Constructor] Resource allocated.\n";
        data = new char[100];
    }
    ~SecretContainer() {
        std::cout << "[Destructor] Resource cleaned up.\n";
        delete[] data;
    }
};

int main() {
    std::cout << "--- Mixing malloc and delete ---\n";

    // 1. ALLOCATION (The C Way)
    // malloc just grabs raw bytes. It does NOT call the constructor.
    // The 'data' pointer inside SecretContainer will be uninitialized junk.
    SecretContainer* myObj = (SecretContainer*)std::malloc(sizeof(SecretContainer));

    if (myObj == NULL) return 1;
    std::cout << "Memory allocated via malloc at: " << myObj << "\n";

    // 2. THE VULNERABILITY (The C++ Way)
    // delete expects the memory to have been allocated with 'new'.
    // It will attempt to:
    //   a) Call the destructor (~SecretContainer)
    //   b) Hand the pointer back to the C++ 'free pool'
    std::cout << "\n[!] Attempting to 'delete' a malloc-ed pointer...\n";
    
    // This often crashes because the destructor tries to delete 'data', 
    // which is currently uninitialized garbage.
    delete myObj; 

    std::cout << "[Success?] If you see this, the heap manager is very forgiving.\n";

    return 0;
}