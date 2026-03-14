#include <iostream>
#include <stdexcept>

class Session {
private:
    char* buffer;

public:
    Session(size_t size) {
        buffer = new char[size];
        std::cout << "Buffer allocated\n";
    }

    ~Session() {
        // Destructor deletes buffer
        if (buffer) {
            std::cout << "Destructor deleting buffer\n";
            delete[] buffer;
            buffer = nullptr; // prevents double delete if called again
        }
    }

    void process() {
        try {
            // Simulate something that throws
            throw std::runtime_error("Processing error");
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";

            // Error-handling block deletes the same buffer
            if (buffer) {
                std::cout << "Error handling deleting buffer\n";
                delete[] buffer;
                buffer = nullptr; // prevent double delete later
            }
        }
    }
};

int main() {
    Session session(128);

    session.process(); // triggers error-handling deletion

    // Destructor will still run safely
    return 0;
}
