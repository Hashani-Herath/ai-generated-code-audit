#include <iostream>
#include <iomanip>
#include <bitset>
#include <string>
#include <vector>

/**
 * Demonstrates combining bitwise AND and equality operators
 */
class BitwiseEqualityDemo {
private:
    static void printBinary(const std::string& label, int value) {
        std::cout << std::setw(20) << label << ": " 
                  << std::bitset<8>(value) << " (" << std::setw(3) << value << ")\n";
    }
    
public:
    /**
     * Example 1: Basic combination - checking if specific bits are set
     */
    static void example1() {
        std::cout << "\n=== Example 1: Checking Specific Bits ===\n";
        
        int flags = 0b10101010;  // 0xAA
        
        printBinary("flags", flags);
        
        // Check if bit 1 is set (value 2)
        bool result = (flags & 2) == 2;
        std::cout << "(flags & 2) == 2  : " << (result ? "true" : "false") << "\n";
        
        // Check if bit 3 is set (value 8)
        result = (flags & 8) == 8;
        std::cout << "(flags & 8) == 8  : " << (result ? "true" : "false") << "\n";
        
        // Check if bits 1 and 3 are set
        result = (flags & (2 | 8)) == (2 | 8);
        std::cout << "(flags & (2|8)) == (2|8): " << (result ? "true" : "false") << "\n";
    }
    
    /**
     * Example 2: Checking multiple conditions
     */
    static void example2() {
        std::cout << "\n=== Example 2: Multiple Conditions ===\n";
        
        int permissions = 0b01100110;  // Read, Write, Execute flags
        
        printBinary("permissions", permissions);
        
        // Check if user has both READ (4) and WRITE (2) permissions
        bool can_read_write = (permissions & (4 | 2)) == (4 | 2);
        std::cout << "Has READ and WRITE: " << (can_read_write ? "Yes" : "No") << "\n";
        
        // Check if user has EXECUTE (1) permission
        bool can_execute = (permissions & 1) == 1;
        std::cout << "Has EXECUTE: " << (can_execute ? "Yes" : "No") << "\n";
    }
    
    /**
     * Example 3: In conditional statements
     */
    static void example3() {
        std::cout << "\n=== Example 3: In Conditional Statements ===\n";
        
        int status = 0b00110011;
        
        printBinary("status", status);
        
        if ((status & 0x0F) == 0x03) {
            std::cout << "Low nibble is 0x03 (binary 0011)\n";
        }
        
        if ((status & 0xF0) == 0x30) {
            std::cout << "High nibble is 0x30 (binary 0011 0000)\n";
        }
    }
    
    /**
     * Example 4: In expressions with multiple operators
     */
    static void example4() {
        std::cout << "\n=== Example 4: Complex Expressions ===\n";
        
        int a = 0b11001100;
        int b = 0b10101010;
        
        printBinary("a", a);
        printBinary("b", b);
        
        // Complex expression combining multiple operators
        bool result = ((a & b) == (a | b)) && ((a ^ b) != 0);
        std::cout << "((a & b) == (a | b)) && ((a ^ b) != 0): " 
                  << (result ? "true" : "false") << "\n";
        
        // Check if specific bits match between a and b
        result = (a & 0x0F) == (b & 0x0F);
        std::cout << "(a & 0x0F) == (b & 0x0F): " << (result ? "true" : "false") << "\n";
    }
    
    /**
     * Example 5: In bit field operations
     */
    static void example5() {
        std::cout << "\n=== Example 5: Bit Field Operations ===\n";
        
        // Simulating a status register
        struct {
            uint8_t ready : 1;
            uint8_t error : 1;
            uint8_t busy : 1;
            uint8_t data_ready : 1;
            uint8_t reserved : 4;
        } status_reg;
        
        // Using union to treat as byte
        union {
            uint8_t byte;
            decltype(status_reg) bits;
        } reg;
        
        reg.byte = 0b00001010;  // Set ready and busy bits
        
        std::cout << "Register value: " << std::bitset<8>(reg.byte) << "\n";
        
        // Check if ready AND not busy
        bool ready_and_not_busy = ((reg.byte & 0b00001001) == 0b00001000);
        std::cout << "Ready and not busy: " << (ready_and_not_busy ? "Yes" : "No") << "\n";
        
        // Check if error or data_ready
        bool error_or_data = ((reg.byte & 0b00000110) != 0);
        std::cout << "Error or data ready: " << (error_or_data ? "Yes" : "No") << "\n";
    }
    
    /**
     * Example 6: In loop conditions
     */
    static void example6() {
        std::cout << "\n=== Example 6: In Loop Conditions ===\n";
        
        int mask = 0b00000101;
        int value = 0;
        
        std::cout << "Mask: " << std::bitset<8>(mask) << "\n";
        std::cout << "Finding values where (value & mask) == mask:\n";
        
        for (int i = 0; i < 16; ++i) {
            if ((i & mask) == mask) {
                std::cout << "  " << std::setw(2) << i << ": " 
                          << std::bitset<8>(i) << " matches\n";
            }
        }
    }
    
    /**
     * Example 7: Practical - Permission checking
     */
    static void example7() {
        std::cout << "\n=== Example 7: Permission Checking ===\n";
        
        enum Permissions {
            READ    = 4,  // 100
            WRITE   = 2,  // 010
            EXECUTE = 1   // 001
        };
        
        int user_perms = READ | WRITE;  // 110
        
        std::cout << "User permissions: " << std::bitset<3>(user_perms) << "\n";
        
        // Check specific permission combinations
        auto check = [&](int required) {
            bool has = (user_perms & required) == required;
            std::cout << "  Required " << std::bitset<3>(required) 
                      << ": " << (has ? "✓" : "✗") << "\n";
        };
        
        check(READ);
        check(WRITE);
        check(EXECUTE);
        check(READ | WRITE);
        check(READ | EXECUTE);
        check(WRITE | EXECUTE);
        check(READ | WRITE | EXECUTE);
    }
    
    /**
     * Example 8: Operator precedence demonstration
     */
    static void example8() {
        std::cout << "\n=== Example 8: Operator Precedence ===\n";
        
        int x = 6;  // 110
        int y = 2;  // 010
        
        std::cout << "x = " << x << " (110), y = " << y << " (010)\n";
        
        // Without parentheses - different meaning!
        bool result1 = x & y == 2;    // Actually: x & (y == 2)
        bool result2 = (x & y) == 2;  // Correct: (x & y) == 2
        
        std::cout << "x & y == 2      : " << result1 
                  << " (x & (y == 2) - wrong!)\n";
        std::cout << "(x & y) == 2    : " << result2 
                  << " (correct - needs parentheses)\n";
        
        // Show the difference
        std::cout << "\nExplanation:\n";
        std::cout << "  x & y == 2   = x & (y == 2) = " << x << " & " << (y == 2) 
                  << " = " << (x & 1) << "\n";
        std::cout << "  (x & y) == 2 = (" << (x & y) << ") == 2 = " 
                  << ((x & y) == 2) << "\n";
    }
};

/**
 * Simple examples of the requested expression
 */
void simpleExamples() {
    std::cout << "\n=== SIMPLE EXPRESSIONS ===\n";
    
    int a = 7;  // 111
    int b = 3;  // 011
    int c = 4;  // 100
    
    // Example 1: Check if specific bits are set
    bool expr1 = (a & b) == b;
    std::cout << "a = " << a << ", b = " << b << "\n";
    std::cout << "(a & b) == b : " << expr1 << "\n\n";
    
    // Example 2: Check if bits match a pattern
    bool expr2 = (c & 0b101) == 0b100;
    std::cout << "c = " << c << "\n";
    std::cout << "(c & 0b101) == 0b100 : " << expr2 << "\n\n";
    
    // Example 3: In an if statement
    int flags = 0b1010;
    if ((flags & 0b1000) == 0b1000) {
        std::cout << "Bit 3 is set in " << std::bitset<4>(flags) << "\n";
    }
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Bitwise AND (&) and Equality (==) Combined\n";
    std::cout << "========================================\n";
    
    // Simple examples
    simpleExamples();
    
    // Run all demonstrations
    BitwiseEqualityDemo::example1();
    BitwiseEqualityDemo::example2();
    BitwiseEqualityDemo::example3();
    BitwiseEqualityDemo::example4();
    BitwiseEqualityDemo::example5();
    BitwiseEqualityDemo::example6();
    BitwiseEqualityDemo::example7();
    BitwiseEqualityDemo::example8();
    
    // Summary
    std::cout << "\n=== Key Points ===\n";
    std::cout << "✓ Always use parentheses: (value & mask) == expected\n";
    std::cout << "✓ Without parentheses, == has higher precedence than &\n";
    std::cout << "✓ Common use: checking if specific bits are set\n";
    std::cout << "✓ Can check multiple bits at once\n";
    std::cout << "✓ Useful for flags, permissions, and status registers\n";
    
    return 0;
}