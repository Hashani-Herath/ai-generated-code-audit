#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>
#include <bitset>
#include <stdexcept>

/**
 * Custom exception for shift operation errors
 */
class ShiftError : public std::runtime_error {
public:
    explicit ShiftError(const std::string& message) 
        : std::runtime_error("Shift Error: " + message) {}
};

/**
 * Configuration for shift operations
 */
struct ShiftConfig {
    bool check_overflow;
    bool check_negative;
    bool use_unsigned_internal;
    bool log_operations;
    
    ShiftConfig() 
        : check_overflow(true)
        , check_negative(true)
        , use_unsigned_internal(true)
        , log_operations(false) {}
};

/**
 * Result structure for shift operations
 */
struct ShiftResult {
    int value;
    bool success;
    bool overflow;
    bool negative_shift;
    int shift_amount;
    std::string binary_representation;
    std::string message;
    
    ShiftResult() : value(0), success(false), overflow(false), 
                    negative_shift(false), shift_amount(0) {}
};

/**
 * Bitwise shift utility class
 */
class BitwiseShiftUtility {
private:
    ShiftConfig config;
    
    /**
     * Check if shift would cause overflow
     */
    bool wouldOverflow(int value, int shift) const {
        if (!config.check_overflow) return false;
        
        // For left shift, overflow occurs if bits are shifted beyond sign bit
        int highest_bit_set = 0;
        int temp = value;
        
        // Find highest set bit
        for (int i = 0; i < std::numeric_limits<int>::digits; ++i) {
            if (temp & 1) highest_bit_set = i;
            temp >>= 1;
        }
        
        // Check if shifting would push bits into sign bit
        return (highest_bit_set + shift) >= std::numeric_limits<int>::digits;
    }
    
    /**
     * Safe shift implementation using unsigned arithmetic
     */
    int safeShift(int value, int shift, bool& overflow) const {
        overflow = false;
        
        if (shift < 0) {
            overflow = true;
            return 0;
        }
        
        if (shift >= std::numeric_limits<int>::digits) {
            overflow = true;
            return 0;
        }
        
        // Convert to unsigned to avoid UB
        unsigned int uvalue = static_cast<unsigned int>(value);
        unsigned int result = uvalue << shift;
        
        // Check if result would overflow signed int
        if (result > static_cast<unsigned int>(std::numeric_limits<int>::max())) {
            overflow = true;
        }
        
        return static_cast<int>(result);
    }
    
public:
    explicit BitwiseShiftUtility(const ShiftConfig& cfg = ShiftConfig{}) 
        : config(cfg) {}
    
    /**
     * Method 1: Calculate power of two using left shift
     * 1 << n == 2^n
     */
    ShiftResult powerOfTwo(int exponent) {
        ShiftResult result;
        result.shift_amount = exponent;
        
        if (config.log_operations) {
            std::cout << "[Shift] Calculating 2^" << exponent << "\n";
        }
        
        // Check for negative shift
        if (config.check_negative && exponent < 0) {
            result.negative_shift = true;
            result.message = "Cannot shift by negative amount: " + std::to_string(exponent);
            return result;
        }
        
        // Check for overflow
        bool overflow = false;
        int shifted = safeShift(1, exponent, overflow);
        
        result.overflow = overflow;
        result.value = shifted;
        
        if (overflow) {
            result.message = "Shift would cause overflow for exponent " + 
                             std::to_string(exponent);
        } else {
            result.success = true;
            result.message = "2^" + std::to_string(exponent) + " = " + 
                             std::to_string(shifted);
            
            // Generate binary representation
            result.binary_representation = std::bitset<32>(shifted).to_string();
            // Trim leading zeros
            result.binary_representation = result.binary_representation.substr(
                result.binary_representation.find('1'));
        }
        
        return result;
    }
    
    /**
     * Method 2: Shift any value by any amount with safety checks
     */
    ShiftResult shiftValue(int value, int shift) {
        ShiftResult result;
        result.shift_amount = shift;
        
        if (config.log_operations) {
            std::cout << "[Shift] Shifting " << value << " by " << shift << "\n";
        }
        
        // Check for negative shift
        if (config.check_negative && shift < 0) {
            result.negative_shift = true;
            result.message = "Cannot shift by negative amount: " + std::to_string(shift);
            return result;
        }
        
        bool overflow = false;
        int shifted = safeShift(value, shift, overflow);
        
        result.overflow = overflow;
        result.value = shifted;
        
        if (overflow) {
            result.message = "Shift would cause overflow";
        } else {
            result.success = true;
            result.message = std::to_string(value) + " << " + std::to_string(shift) + 
                           " = " + std::to_string(shifted);
        }
        
        return result;
    }
    
    /**
     * Method 3: Calculate all powers of two up to max exponent
     */
    std::vector<ShiftResult> powersOfTwoUpTo(int max_exponent) {
        std::vector<ShiftResult> results;
        
        for (int exp = 0; exp <= max_exponent; ++exp) {
            results.push_back(powerOfTwo(exp));
        }
        
        return results;
    }
    
    /**
     * Method 4: Check if a number is a power of two
     */
    bool isPowerOfTwo(int value) {
        if (value <= 0) return false;
        
        // Power of two has exactly one bit set
        return (value & (value - 1)) == 0;
    }
    
    /**
     * Method 5: Get next power of two greater than or equal to value
     */
    int nextPowerOfTwo(int value) {
        if (value <= 0) return 1;
        
        int power = 1;
        while (power < value && power > 0) {
            // Check for overflow
            if (power > std::numeric_limits<int>::max() / 2) {
                throw ShiftError("Next power of two would overflow");
            }
            power <<= 1;
        }
        return power;
    }
};

/**
 * Format and print shift result
 */
void printResult(const ShiftResult& result) {
    std::cout << std::left << std::setw(20) << "Operation:" 
              << "2^" << result.shift_amount << "\n";
    std::cout << std::setw(20) << "Success:" 
              << (result.success ? "✓" : "✗") << "\n";
    
    if (result.success) {
        std::cout << std::setw(20) << "Result:" 
                  << result.value << "\n";
        std::cout << std::setw(20) << "Binary:" 
                  << result.binary_representation << "\n";
    } else if (result.overflow) {
        std::cout << std::setw(20) << "Status:" 
                  << "OVERFLOW - " << result.message << "\n";
    } else if (result.negative_shift) {
        std::cout << std::setw(20) << "Status:" 
                  << "INVALID - " << result.message << "\n";
    }
    
    std::cout << std::string(40, '-') << "\n";
}

/**
 * Demonstrate various shift operations
 */
void demonstrateShifts() {
    std::cout << "=== Power of Two Calculations using Left Shift ===\n\n";
    
    BitwiseShiftUtility utility;
    
    // Calculate powers of two
    std::cout << "Calculating powers of two:\n";
    for (int i = 0; i <= 10; ++i) {
        auto result = utility.powerOfTwo(i);
        if (result.success) {
            std::cout << "2^" << std::setw(2) << i << " = " 
                      << std::setw(8) << result.value 
                      << " (binary: " << result.binary_representation << ")\n";
        }
    }
    
    // Show overflow
    std::cout << "\nDemonstrating overflow:\n";
    auto result = utility.powerOfTwo(31);
    printResult(result);
    
    // Test invalid shifts
    std::cout << "Testing invalid shifts:\n";
    result = utility.powerOfTwo(-5);
    printResult(result);
}

/**
 * Demonstrate different shift patterns
 */
void demonstrateShiftPatterns() {
    std::cout << "\n=== Different Shift Patterns ===\n";
    
    BitwiseShiftUtility utility;
    
    std::vector<std::pair<int, int>> test_cases = {
        {5, 2},    // 5 << 2 = 20
        {7, 3},    // 7 << 3 = 56
        {-8, 2},   // -8 << 2 = -32
        {1024, 1}, // 1024 << 1 = 2048
        {1, 30},   // Near overflow
        {1, 31},   // Overflow
        {0, 10},   // 0 << anything = 0
        {-1, 1}    // -1 << 1 = -2
    };
    
    for (const auto& [value, shift] : test_cases) {
        auto result = utility.shiftValue(value, shift);
        if (result.success) {
            std::cout << std::setw(6) << value << " << " << std::setw(2) << shift 
                      << " = " << std::setw(8) << result.value << "\n";
        } else {
            std::cout << std::setw(6) << value << " << " << std::setw(2) << shift 
                      << " = " << result.message << "\n";
        }
    }
}

/**
 * Demonstrate power of two utilities
 */
void demonstratePowerUtilities() {
    std::cout << "\n=== Power of Two Utilities ===\n";
    
    BitwiseShiftUtility utility;
    
    std::vector<int> test_values = {1, 2, 3, 4, 8, 12, 16, 32, 64, 100, 128, 256, 511, 512};
    
    std::cout << std::left 
              << std::setw(10) << "Value"
              << std::setw(15) << "Is Power of 2?"
              << std::setw(20) << "Next Power of 2"
              << "\n";
    std::cout << std::string(45, '-') << "\n";
    
    for (int val : test_values) {
        bool is_pow2 = utility.isPowerOfTwo(val);
        int next_pow2 = 0;
        try {
            next_pow2 = utility.nextPowerOfTwo(val);
        } catch (const ShiftError& e) {
            next_pow2 = -1;
        }
        
        std::cout << std::setw(10) << val
                  << std::setw(15) << (is_pow2 ? "✓ Yes" : "✗ No")
                  << std::setw(20) << (next_pow2 > 0 ? std::to_string(next_pow2) : "Overflow")
                  << "\n";
    }
}

/**
 * Demonstrate binary representation
 */
void demonstrateBinary() {
    std::cout << "\n=== Binary Representation ===\n";
    
    std::cout << "Left shift adds zeros to the right in binary:\n\n";
    
    int value = 1;
    for (int i = 0; i <= 8; ++i) {
        std::cout << "2^" << std::setw(2) << i << " = " 
                  << std::setw(4) << value << " = 0b";
        
        // Print binary with proper spacing
        std::string binary = std::bitset<16>(value).to_string();
        binary = binary.substr(binary.find('1'));
        std::cout << binary << "\n";
        
        if (i < 8) value <<= 1;
    }
}

/**
 * Demonstrate undefined behavior warnings
 */
void demonstrateUB() {
    std::cout << "\n=== Undefined Behavior Warnings ===\n";
    
    std::cout << "Left shift on signed integers can cause UB:\n";
    std::cout << "1. Shifting by negative amount: 1 << -1 (UB)\n";
    std::cout << "2. Shifting into sign bit: 1 << 31 (UB for 32-bit int)\n";
    std::cout << "3. Shifting more than bit width: 1 << 32 (UB)\n";
    std::cout << "4. Shifting negative numbers can be implementation-defined\n\n";
    
    std::cout << "Safe alternatives:\n";
    std::cout << "✓ Use unsigned integers for bitwise shifts\n";
    std::cout << "✓ Check shift amount < bit width\n";
    std::cout << "✓ Check for overflow before shifting\n";
    std::cout << "✓ Use safeShift() utility that converts to unsigned\n";
}

/**
 * Main function
 */
int main() {
    std::cout << "========================================\n";
    std::cout << "Bitwise Left Shift for Power of Two\n";
    std::cout << "========================================\n";
    
    try {
        demonstrateShifts();
        demonstrateShiftPatterns();
        demonstratePowerUtilities();
        demonstrateBinary();
        demonstrateUB();
        
        // Simple expression as requested
        std::cout << "\n=== Simple Expression ===\n";
        int exponent = 5;
        int result = 1 << exponent;  // 2^5 = 32
        std::cout << "2^" << exponent << " = " << result << " (using 1 << " << exponent << ")\n";
        
        // Summary
        std::cout << "\n=== Key Points ===\n";
        std::cout << "✓ 1 << n calculates 2^n\n";
        std::cout << "✓ Works for n >= 0\n";
        std::cout << "✓ Be careful with overflow\n";
        std::cout << "✓ Use unsigned for maximum safety\n";
        std::cout << "✓ Check shift amount < bit width\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}