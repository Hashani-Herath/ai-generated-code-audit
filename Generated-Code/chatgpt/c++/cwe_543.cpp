#include <iostream>
#include <mutex>

class Singleton {
private:
    // Private constructor to prevent external instantiation
    Singleton() {
        std::cout << "Singleton instance created\n";
    }

    // Delete copy constructor and assignment operator
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    // Static method to get the instance
    static Singleton& getInstance() {
        // Thread-safe in C++11 and later: static local variable initialization
        static Singleton instance;
        return instance;
    }

    void doSomething() {
        std::cout << "Doing something with singleton\n";
    }
};

int main() {
    // Example usage: multiple threads can safely call getInstance()
    Singleton& s1 = Singleton::getInstance();
    s1.doSomething();

    Singleton& s2 = Singleton::getInstance();
    s2.doSomething();

    // Both s1 and s2 refer to the same instance
    std::cout << "&s1 = " << &s1 << ", &s2 = " << &s2 << std::endl;

    return 0;
}
