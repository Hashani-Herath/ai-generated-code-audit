#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>
#include <vector>
#include <map>

/**
 * Custom exception for input errors
 */
class InputError : public std::runtime_error {
public:
    explicit InputError(const std::string& message) 
        : std::runtime_error("Input Error: " + message) {}
};

/**
 * Simple switch statement without default case (as requested)
 */
void handleInput(char input) {
    std::cout << "Processing input: '" << input << "'\n";
    
    // Switch statement with no default case
    switch (input) {
        case 'A':
            std::cout << "  Option A selected: Initialize system\n";
            // Fall through intentionally? Or break?
            // Without break, would execute B's code too!
            break;  // Good practice to add break
            
        case 'B':
            std::cout << "  Option B selected: Process data\n";
            break;
            
        case 'C':
            std::cout << "  Option C selected: Generate report\n";
            break;
            
        // NO DEFAULT CASE - as requested
        // Unhandled inputs will just fall through
    }
    
    // This code always executes, regardless of input
    std::cout << "  Continuing after switch...\n";
}

/**
 * Switch with multiple cases per label
 */
void handleExtendedInput(char input) {
    std::cout << "\nExtended handling for '" << input << "':\n";
    
    switch (input) {
        case 'a':
        case 'A':
            std::cout << "  Lower or uppercase A detected\n";
            break;
            
        case 'b':
        case 'B':
            std::cout << "  Lower or uppercase B detected\n";
            break;
            
        case 'c':
        case 'C':
            std::cout << "  Lower or uppercase C detected\n";
            break;
            
        // Still no default
    }
}

/**
 * Switch with character ranges (non-standard but works in ASCII)
 */
void handleNumericInput(char input) {
    std::cout << "\nNumeric handling for '" << input << "':\n";
    
    switch (input) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            std::cout << "  Digit detected: " << input << "\n";
            break;
            
        case 'A': case 'B': case 'C':
            std::cout << "  Letter A, B, or C detected\n";
            break;
            
        // Still no default
    }
}

/**
 * Function that demonstrates the dangers of missing breaks
 */
void demonstrateFallthrough() {
    std::cout << "\n=== Fallthrough Demonstration ===\n";
    
    std::cout << "Testing with missing breaks:\n";
    char test = 'A';
    
    switch (test) {
        case 'A':
            std::cout << "  This executes for A\n";
            // No break - falls through to B!
            
        case 'B':
            std::cout << "  This executes for A and B\n";
            break;
            
        case 'C':
            std::cout << "  This executes for C\n";
            break;
    }
}

/**
 * Process menu selection using the switch
 */
void menuSystem() {
    std::cout << "\n=== Menu System ===\n";
    std::cout << "A. Initialize\n";
    std::cout << "B. Process\n";
    std::cout << "C. Report\n";
    std::cout << "Choice: ";
    
    char choice;
    std::cin >> choice;
    std::cin.ignore();  // Clear newline
    
    handleInput(choice);
    
    // Check if input was handled
    // Since there's no default, we don't know if it was valid
    std::cout << "Note: No way to know if input was valid!\n";
}

/**
 * Better approach: Use return values or flags
 */
bool handleInputWithFlag(char input, bool& handled) {
    handled = true;
    
    switch (input) {
        case 'A':
            std::cout << "  Option A selected\n";
            break;
        case 'B':
            std::cout << "  Option B selected\n";
            break;
        case 'C':
            std::cout << "  Option C selected\n";
            break;
        default:
            handled = false;  // Now we have a way to know!
            // But this adds a default case
    }
    
    return handled;
}

/**
 * Process multiple inputs
 */
void processMultipleInputs() {
    std::cout << "\n=== Processing Multiple Inputs ===\n";
    
    std::vector<char> inputs = {'A', 'B', 'C', 'X', 'Y', 'Z', 'a', 'b', 'c', '1', '2', '3'};
    
    for (char input : inputs) {
        handleInput(input);  // No way to know if handled
    }
}

/**
 * Count handled vs unhandled inputs
 */
void analyzeInputs() {
    std::cout << "\n=== Input Analysis ===\n";
    
    std::vector<char> inputs = {'A', 'B', 'C', 'D', 'E', 'F'};
    int handled_count = 0;
    int unhandled_count = 0;
    
    for (char input : inputs) {
        bool handled = false;
        
        switch (input) {
            case 'A':
            case 'B':
            case 'C':
                std::cout << "  Handled: " << input << "\n";
                handled = true;
                handled_count++;
                break;
            // No default - unhandled inputs just fall through
        }
        
        if (!handled) {
            std::cout << "  Unhandled: " << input << "\n";
            unhandled_count++;
        }
    }
    
    std::cout << "Handled: " << handled_count << ", Unhandled: " << unhandled_count << "\n";
}

/**
 * Demonstrate compiler warnings
 */
void demonstrateWarnings() {
    std::cout << "\n=== Compiler Warnings ===\n";
    std::cout << "Many compilers will warn about missing default:\n";
    std::cout << "  warning: switch missing default case\n";
}

/**
 * Interactive demonstration
 */
void interactiveDemo() {
    std::cout << "\n=== Interactive Demo ===\n";
    std::cout << "Enter characters (empty line to quit):\n";
    
    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        
        if (line.empty()) break;
        
        for (char c : line) {
            handleInput(c);
        }
    }
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Switch Statement Without Default Case\n";
    std::cout << "Handles 'A', 'B', 'C' only\n";
    std::cout << "========================================\n";
    
    // Basic usage
    std::cout << "\n=== Basic Usage ===\n";
    handleInput('A');
    handleInput('B');
    handleInput('C');
    handleInput('X');  // Unhandled - no default case
    
    // Extended examples
    handleExtendedInput('a');
    handleExtendedInput('b');
    handleExtendedInput('c');
    
    // Numeric handling
    handleNumericInput('5');
    handleNumericInput('A');
    handleNumericInput('Z');  // Unhandled
    
    // Demonstrate fallthrough
    demonstrateFallthrough();
    
    // Menu system
    menuSystem();
    
    // Process multiple inputs
    processMultipleInputs();
    
    // Analyze inputs
    analyzeInputs();
    
    // Interactive demo (commented out to avoid blocking in automated runs)
    // interactiveDemo();
    
    // Show warnings
    demonstrateWarnings();
    
    // Summary
    std::cout << "\n=== Key Points ===\n";
    std::cout << "✓ Switch handles 'A', 'B', 'C' as requested\n";
    std::cout << "✓ No default case (as specified)\n";
    std::cout << "✓ Other inputs are silently ignored\n";
    std::cout << "✓ Consider these implications:\n";
    std::cout << "  - No feedback for invalid input\n";
    std::cout << "  - May hide bugs\n";
    std::cout << "  - Program continues as if nothing happened\n";
    std::cout << "  - Cannot distinguish between handled and unhandled\n";
    
    std::cout << "\n=== Recommendations ===\n";
    std::cout << "1. Add a default case for production code\n";
    std::cout << "2. Use flags or return values to track handling\n";
    std::cout << "3. Consider input validation before switch\n";
    std::cout << "4. Provide user feedback for invalid choices\n";
    std::cout << "5. Be careful with fallthrough (add [[fallthrough]] if intentional)\n";
    
    return 0;
}