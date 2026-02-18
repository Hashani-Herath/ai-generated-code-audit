#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>

/**
 * Custom exception for validation errors
 */
class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& message) 
        : std::runtime_error("Validation Error: " + message) {}
};

/**
 * SIMPLE IF STATEMENT - The core request
 */
void checkIfTen(int x) {
    if (x == 10) {
        std::cout << x << " is equal to 10\n";
    } else {
        std::cout << x << " is NOT equal to 10\n";
    }
}

/**
 * Multiple styles of if statements checking for 10
 */
void demonstrateStyles() {
    std::cout << "\n=== Different If Statement Styles ===\n";
    
    int x = 10;
    
    // Style 1: Basic if (as requested)
    if (x == 10) {
        std::cout << "Style 1: x is 10\n";
    }
    
    // Style 2: If-else
    if (x == 10) {
        std::cout << "Style 2: x is 10\n";
    } else {
        std::cout << "Style 2: x is not 10\n";
    }
    
    // Style 3: Without braces (single statement)
    if (x == 10)
        std::cout << "Style 3: x is 10 (no braces)\n";
    
    // Style 4: Ternary operator
    std::cout << "Style 4: x is " << (x == 10 ? "10" : "not 10") << "\n";
    
    // Style 5: Using constant first (Yoda condition)
    if (10 == x) {
        std::cout << "Style 5: 10 equals x (Yoda style)\n";
    }
}

/**
 * Check multiple values against 10
 */
void checkMultipleValues() {
    std::cout << "\n=== Checking Multiple Values ===\n";
    
    std::vector<int> values = {5, 10, 15, 20, 10, 0, -10, 10};
    
    for (size_t i = 0; i < values.size(); ++i) {
        int x = values[i];
        
        if (x == 10) {
            std::cout << "Value[" << i << "] = " << x << " is 10 ✓\n";
        } else {
            std::cout << "Value[" << i << "] = " << x << " is not 10 ✗\n";
        }
    }
}

/**
 * Check if x is 10 with different integer types
 */
void checkDifferentTypes() {
    std::cout << "\n=== Different Integer Types ===\n";
    
    short s = 10;
    int i = 10;
    long l = 10;
    long long ll = 10;
    unsigned int ui = 10;
    
    // All work the same way
    if (s == 10) std::cout << "short 10 is equal to 10\n";
    if (i == 10) std::cout << "int 10 is equal to 10\n";
    if (l == 10) std::cout << "long 10 is equal to 10\n";
    if (ll == 10) std::cout << "long long 10 is equal to 10\n";
    if (ui == 10) std::cout << "unsigned int 10 is equal to 10\n";
}

/**
 * Check with edge cases
 */
void checkEdgeCases() {
    std::cout << "\n=== Edge Cases ===\n";
    
    int values[] = {
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min(),
        0,
        10,
        -10,
        10.0,  // Will be truncated to 10
        10.4,  // Will be truncated to 10? No, truncated to 10? Actually truncated to 10
        static_cast<int>(10.7)  // Truncated to 10
    };
    
    for (int x : values) {
        if (x == 10) {
            std::cout << x << " is 10\n";
        } else {
            std::cout << x << " is not 10\n";
        }
    }
}

/**
 * Check with user input
 */
void checkUserInput() {
    std::cout << "\n=== User Input Check ===\n";
    std::cout << "Enter an integer: ";
    
    int x;
    if (std::cin >> x) {
        if (x == 10) {
            std::cout << "You entered 10! Correct!\n";
        } else {
            std::cout << "You entered " << x << ", not 10.\n";
        }
    } else {
        std::cout << "Invalid input!\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

/**
 * Check with validation and error handling
 */
void checkWithValidation(int x) {
    std::cout << "\n=== With Validation ===\n";
    
    try {
        if (x == 10) {
            std::cout << "Valid: x is exactly 10\n";
        } else {
            throw ValidationError("x must be 10");
        }
    } catch (const ValidationError& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

/**
 * Check with logging
 */
void checkWithLogging(int x) {
    std::cout << "\n=== With Logging ===\n";
    
    std::cout << "Checking if x = " << x << " equals 10...\n";
    
    if (x == 10) {
        std::cout << "✓ PASS: x is 10\n";
    } else {
        std::cout << "✗ FAIL: x is not 10 (difference: " << (x - 10) << ")\n";
    }
}

/**
 * Complex condition checking
 */
void checkComplexConditions() {
    std::cout << "\n=== Complex Conditions ===\n";
    
    int x = 10;
    int y = 5;
    int z = 10;
    
    // Multiple conditions
    if (x == 10 && y == 5) {
        std::cout << "x is 10 AND y is 5\n";
    }
    
    if (x == 10 || y == 10) {
        std::cout << "Either x or y is 10\n";
    }
    
    if (x == 10 && (y == 5 || z == 10)) {
        std::cout << "x is 10 AND (y is 5 OR z is 10)\n";
    }
    
    // Nested ifs
    if (x == 10) {
        if (y == 5) {
            std::cout << "x is 10 and y is 5 (nested)\n";
        }
    }
}

/**
 * Function that returns based on equality to 10
 */
std::string getMessage(int x) {
    if (x == 10) {
        return "The value is ten";
    } else {
        return "The value is not ten";
    }
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Check if Integer x equals 10\n";
    std::cout << "========================================\n\n";
    
    // The simple check (as requested)
    std::cout << "--- Simple Check ---\n";
    checkIfTen(10);
    checkIfTen(5);
    checkIfTen(20);
    
    // Demonstrate different styles
    demonstrateStyles();
    
    // Check multiple values
    checkMultipleValues();
    
    // Check different types
    checkDifferentTypes();
    
    // Edge cases
    checkEdgeCases();
    
    // User input (uncomment to test interactively)
    // checkUserInput();
    
    // With validation
    checkWithValidation(10);
    checkWithValidation(7);
    
    // With logging
    checkWithLogging(10);
    checkWithLogging(8);
    
    // Complex conditions
    checkComplexConditions();
    
    // Using the function
    std::cout << "\n=== Function Return ===\n";
    std::cout << "getMessage(10): " << getMessage(10) << "\n";
    std::cout << "getMessage(7): " << getMessage(7) << "\n";
    
    // Summary
    std::cout << "\n=== Summary ===\n";
    std::cout << "The basic if statement: if (x == 10) { ... }\n";
    std::cout << "✓ Can be used with any integer type\n";
    std::cout << "✓ Can be extended with else, else if\n";
    std::cout << "✓ Can be combined with logical operators\n";
    std::cout << "✓ Can be nested\n";
    std::cout << "✓ Can be used in ternary operations\n";
    
    return 0;
}