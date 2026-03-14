#include <iostream>
#include <stdexcept>

class VulnerableProcessor {
private:
    int* data_buffer;

public:
    VulnerableProcessor() {
        // 1. Allocate memory on the heap
        data_buffer = new int[100];
        std::cout << "Memory allocated at address: " << data_buffer << "\n";
    }

    // The Destructor
    ~VulnerableProcessor() {
        // 3. THE VULNERABILITY: Double Free
        // If process_data() failed, this will attempt to delete memory 
        // that has already been returned to the system.
        std::cout << "Destructor executing. Deleting address: " << data_buffer << "\n";
        delete[] data_buffer; 
    }

    void process_data() {
        try {
            std::cout << "Processing data...\n";
            // Simulate a critical failure during processing
            throw std::runtime_error("Data corruption detected!");
            
        } catch (const std::exception& e) {
            std::cerr << "Error caught: " << e.what() << "\n";
            
            // 2. The Error Handler Cleanup
            // The developer tries to be responsible by cleaning up memory early.
            std::cout << "Error handler deleting address: " << data_buffer << "\n";
            delete[] data_buffer; 
            
            // CRITICAL FLAW: The pointer is not set to nullptr after deletion.
            // data_buffer still holds the memory address, even though we don't own it anymore.
        }
    }
};

int main() {
    std::cout << "--- Starting Process ---\n";
    {
        VulnerableProcessor processor;
        processor.process_data(); // This triggers the first delete
        
    } // The object goes out of scope here, triggering the destructor (the second delete)
    
    std::cout << "--- Process Finished --