#include <iostream>

// 1. Define a simple class hierarchy
class Base {
public:
    virtual ~Base() = default; // Always good practice for polymorphic base classes
    void identify() { std::cout << "I am the Base.\n"; }
};

class Derived : public Base {
public:
    int specific_data = 42;
    void identifyDerived() { 
        std::cout << "I am the Derived class. Data: " << specific_data << "\n"; 
    }
};

int main() {
    // --- The Safe Scenario ---
    std::cout << "--- Safe Cast ---\n";
    Derived actual_derived;
    Base* base_ptr_to_derived = &actual_derived; // Upcast (Implicit, always safe)

    // Downcast using static_cast (Safe ONLY because we know it's a Derived object)
    Derived* safe_derived_ptr = static_cast<Derived*>(base_ptr_to_derived);
    safe_derived_ptr->identifyDerived(); 


    // --- The Dangerous Scenario ---
    std::cout << "\n--- Dangerous Cast ---\n";
    Base actual_base;
    Base* true_base_ptr = &actual_base;

    // Downcast using static_cast (UNSAFE: It's not actually a Derived object!)
    Derived* unsafe_derived_ptr = static_cast<Derived*>(true_base_ptr);
    
    // 2. UNDEFINED BEHAVIOR: 
    // The program will attempt to read 'specific_data' from memory that doesn't exist.
    // In a real application, this might crash or print garbage data.
    std::cout << "Attempting to access invalid memory...\n";
    // unsafe_derived_ptr->identifyDerived(); // Uncommenting this is dangerous!

    return 0;
}