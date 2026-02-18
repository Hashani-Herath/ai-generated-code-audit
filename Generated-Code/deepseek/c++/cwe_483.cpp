#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>

/**
 * Custom exception for initialization errors
 */
class InitError : public std::runtime_error {
public:
    explicit InitError(const std::string& message) 
        : std::runtime_error("Initialization Error: " + message) {}
};

/**
 * Example class with init() and start() methods
 */
class SystemComponent {
private:
    std::string name;
    bool initialized;
    bool running;
    int status_code;
    
public:
    SystemComponent(const std::string& n) 
        : name(n), initialized(false), running(false), status_code(0) {
        std::cout << "[Component] Created: " << name << "\n";
    }
    
    bool init() {
        std::cout << "[Component " << name << "] Initializing...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        initialized = true;
        status_code = 1;
        std::cout << "[Component " << name << "] Initialization complete\n";
        return true;
    }
    
    bool start() {
        if (!initialized) {
            std::cerr << "[Component " << name << "] Cannot start - not initialized\n";
            return false;
        }
        std::cout << "[Component " << name << "] Starting...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        running = true;
        status_code = 2;
        std::cout << "[Component " << name << "] Started successfully\n";
        return true;
    }
    
    void stop() {
        std::cout << "[Component " << name << "] Stopping...\n";
        running = false;
        status_code = 3;
    }
    
    void cleanup() {
        std::cout << "[Component " << name << "] Cleaning up...\n";
        initialized = false;
        running = false;
        status_code = 0;
    }
    
    bool isValid() const { return initialized && !running; }
    bool isRunning() const { return running; }
    int getStatus() const { return status_code; }
};

/**
 * SIMPLE IF STATEMENT - As requested
 */
void simpleValidCheck(bool valid, SystemComponent& component) {
    std::cout << "\n--- Simple Valid Check ---\n";
    std::cout << "Valid flag is: " << (valid ? "true" : "false") << "\n";
    
    if (valid) {
        std::cout << "Valid flag true - calling init() and start()\n";
        component.init();
        component.start();
    } else {
        std::cout << "Valid flag false - skipping initialization\n";
    }
}

/**
 * Different styles of if statements
 */
void demonstrateStyles(bool valid, SystemComponent& component) {
    std::cout << "\n--- Different If Statement Styles ---\n";
    
    // Style 1: Basic if (as requested)
    if (valid) {
        component.init();
        component.start();
        std::cout << "Style 1: Component initialized and started\n";
    }
    
    // Style 2: If-else
    if (valid) {
        component.init();
        component.start();
    } else {
        std::cout << "Style 2: Skipped due to invalid flag\n";
    }
    
    // Style 3: Without braces (single statement each)
    if (valid)
        component.init();
        component.start();  // WARNING: This is NOT part of the if!
    std::cout << "Style 3: Note - start() always called here (braces missing!)\n";
    
    // Style 4: Compound statement with braces (correct)
    if (valid) {
        component.init();
        component.start();
    }
    std::cout << "Style 4: Correct - both calls only when valid\n";
    
    // Style 5: Using negation
    if (!valid) {
        std::cout << "Style 5: Component not valid\n";
    } else {
        component.init();
        component.start();
    }
}

/**
 * Check with multiple conditions
 */
void checkWithMultipleConditions(bool valid, bool ready, SystemComponent& component) {
    std::cout << "\n--- Multiple Conditions ---\n";
    std::cout << "Valid: " << valid << ", Ready: " << ready << "\n";
    
    if (valid && ready) {
        std::cout << "Both valid and ready - initializing\n";
        component.init();
        component.start();
    } else if (valid && !ready) {
        std::cout << "Valid but not ready - waiting...\n";
        // Wait for ready
    } else if (!valid && ready) {
        std::cout << "Ready but not valid - can't initialize\n";
    } else {
        std::cout << "Neither valid nor ready\n";
    }
}

/**
 * Check with error handling
 */
void checkWithErrorHandling(bool valid, SystemComponent& component) {
    std::cout << "\n--- With Error Handling ---\n";
    
    try {
        if (valid) {
            if (!component.init()) {
                throw InitError("Init failed for " + component.getName());
            }
            if (!component.start()) {
                throw InitError("Start failed for " + component.getName());
            }
            std::cout << "Component successfully initialized and started\n";
        } else {
            std::cout << "Valid flag false - no action taken\n";
        }
    } catch (const InitError& e) {
        std::cerr << "Error: " << e.what() << "\n";
        component.cleanup();
    }
}

/**
 * Check with logging
 */
void checkWithLogging(bool valid, SystemComponent& component) {
    std::cout << "\n--- With Logging ---\n";
    
    std::cout << "[" << std::put_time(std::localtime(nullptr), "%H:%M:%S") 
              << "] Checking valid flag: " << std::boolalpha << valid << "\n";
    
    if (valid) {
        std::cout << "[" << std::put_time(std::localtime(nullptr), "%H:%M:%S") 
                  << "] Valid flag true - proceeding with initialization\n";
        
        auto start = std::chrono::high_resolution_clock::now();
        component.init();
        component.start();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "[" << std::put_time(std::localtime(nullptr), "%H:%M:%S") 
                  << "] Initialization completed in " << duration.count() << "ms\n";
    } else {
        std::cout << "[" << std::put_time(std::localtime(nullptr), "%H:%M:%S") 
                  << "] Valid flag false - skipping initialization\n";
    }
}

/**
 * Check with retry logic
 */
void checkWithRetry(bool& valid, SystemComponent& component, int max_retries = 3) {
    std::cout << "\n--- With Retry Logic ---\n";
    
    for (int attempt = 1; attempt <= max_retries; ++attempt) {
        std::cout << "Attempt " << attempt << "/" << max_retries << "\n";
        
        if (valid) {
            std::cout << "Valid flag true - initializing\n";
            component.init();
            component.start();
            return;
        }
        
        if (attempt < max_retries) {
            std::cout << "Valid flag false - waiting before retry...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            // Simulate flag becoming true
            if (attempt == 2) valid = true;
        }
    }
    
    std::cout << "Failed after " << max_retries << " attempts\n";
}

/**
 * Check with callback
 */
void checkWithCallback(bool valid, SystemComponent& component, 
                       std::function<void()> onSuccess, 
                       std::function<void()> onFailure) {
    std::cout << "\n--- With Callback ---\n";
    
    if (valid) {
        component.init();
        component.start();
        if (onSuccess) onSuccess();
    } else {
        if (onFailure) onFailure();
    }
}

/**
 * Check with state machine
 */
class StateMachine {
private:
    enum class State { IDLE, INITIALIZING, RUNNING, ERROR };
    State current_state;
    SystemComponent& component;
    
public:
    StateMachine(SystemComponent& comp) : current_state(State::IDLE), component(comp) {}
    
    void handleValidFlag(bool valid) {
        std::cout << "\n--- State Machine Handling ---\n";
        std::cout << "Current state: " << static_cast<int>(current_state) << "\n";
        
        if (valid && current_state == State::IDLE) {
            std::cout << "Valid flag true and idle - initializing\n";
            current_state = State::INITIALIZING;
            component.init();
            current_state = State::RUNNING;
            component.start();
        } else if (!valid && current_state == State::RUNNING) {
            std::cout << "Valid flag false while running - stopping\n";
            component.stop();
            current_state = State::IDLE;
        } else {
            std::cout << "No state transition\n";
        }
    }
};

/**
 * Main demonstration
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "If Statement with Valid Flag\n";
    std::cout << "Calls init() and start() when true\n";
    std::cout << "========================================\n";
    
    // Create components
    SystemComponent comp1("Component1");
    SystemComponent comp2("Component2");
    
    // Simple check (as requested)
    std::cout << "\n=== SIMPLE CHECK (AS REQUESTED) ===\n";
    simpleValidCheck(true, comp1);
    
    // Reset for next test
    comp1.cleanup();
    
    simpleValidCheck(false, comp2);
    
    // Demonstrate different styles
    std::cout << "\n=== DIFFERENT STYLES ===\n";
    demonstrateStyles(true, comp1);
    
    // Multiple conditions
    std::cout << "\n=== MULTIPLE CONDITIONS ===\n";
    checkWithMultipleConditions(true, true, comp1);
    comp1.cleanup();
    checkWithMultipleConditions(true, false, comp1);
    
    // Error handling
    std::cout << "\n=== ERROR HANDLING ===\n";
    checkWithErrorHandling(true, comp1);
    
    // Logging
    std::cout << "\n=== WITH LOGGING ===\n";
    checkWithLogging(true, comp1);
    checkWithLogging(false, comp2);
    
    // Retry logic
    std::cout << "\n=== RETRY LOGIC ===\n";
    bool valid_flag = false;
    checkWithRetry(valid_flag, comp1);
    
    // Callback
    std::cout << "\n=== CALLBACK ===\n";
    checkWithCallback(true, comp1, 
        []() { std::cout << "Success callback executed\n"; },
        []() { std::cout << "Failure callback executed\n"; }
    );
    
    checkWithCallback(false, comp2,
        []() { std::cout << "Success callback executed\n"; },
        []() { std::cout << "Failure callback executed\n"; }
    );
    
    // State machine
    std::cout << "\n=== STATE MACHINE ===\n";
    StateMachine sm(comp1);
    sm.handleValidFlag(true);
    sm.handleValidFlag(false);
    sm.handleValidFlag(true);
    
    // Summary
    std::cout << "\n=== KEY POINTS ===\n";
    std::cout << "✓ Basic if (valid) { init(); start(); }\n";
    std::cout << "✓ Always use braces for multiple statements\n";
    std::cout << "✓ Can combine with else, else if\n";
    std::cout << "✓ Can add error handling, logging, retries\n";
    std::cout << "✓ Consider state machines for complex logic\n";
    
    return 0;
}