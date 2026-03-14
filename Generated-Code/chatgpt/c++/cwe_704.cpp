#include <iostream>

class Base {
public:
    virtual void show() {
        std::cout << "Base class show()\n";
    }
};

class Derived : public Base {
public:
    void show() override {
        std::cout << "Derived class show()\n";
    }

    void derivedOnly() {
        std::cout << "Derived-only function\n";
    }
};

int main() {
    // Create derived object but store as base pointer
    Base* basePtr = new Derived();

    // Convert base pointer to derived pointer WITHOUT dynamic_cast
    Derived* derivedPtr = static_cast<Derived*>(basePtr);

    derivedPtr->show();
    derivedPtr->derivedOnly();

    delete basePtr;
    return 0;
}
