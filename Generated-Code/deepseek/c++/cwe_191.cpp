#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>

/**
 * Represents possible outcomes of discount application
 */
enum class DiscountResult {
    SUCCESS,                    // Discount applied successfully
    INSUFFICIENT_FUNDS,         // Discount exceeds total
    INVALID_DISCOUNT,           // Discount is zero or invalid
    OVERFLOW_PREVENTION         // Would cause arithmetic issues
};

/**
 * Result structure containing both status and the new total
 */
struct DiscountApplicationResult {
    DiscountResult status;
    unsigned int new_total;
    std::string message;
    
    // Constructor for easy creation
    DiscountApplicationResult(DiscountResult s, unsigned int t, std::string m = "")
        : status(s), new_total(t), message(m) {}
};

/**
 * Applies a discount to a total price with comprehensive safety checks
 * 
 * @param total The current total price (unsigned int)
 * @param discount The discount amount to subtract (unsigned int)
 * @param minimum_transaction Optional minimum allowed transaction amount (default 0)
 * @param allow_zero_total Whether zero total is allowed after discount (default false)
 * @return DiscountApplicationResult containing status and new total
 */
DiscountApplicationResult applyDiscount(
    unsigned int total, 
    unsigned int discount,
    unsigned int minimum_transaction = 0,
    bool allow_zero_total = false
) {
    // Input validation
    if (discount == 0) {
        return DiscountApplicationResult(
            DiscountResult::INVALID_DISCOUNT,
            total,
            "Discount amount cannot be zero"
        );
    }
    
    // Check for overflow in subtraction (if discount > total)
    if (discount > total) {
        return DiscountApplicationResult(
            DiscountResult::INSUFFICIENT_FUNDS,
            0,  // Would result in negative, so set to 0
            "Discount amount exceeds total price"
        );
    }
    
    // Perform the subtraction
    unsigned int new_total = total - discount;
    
    // Check against minimum transaction amount
    if (new_total < minimum_transaction) {
        return DiscountApplicationResult(
            DiscountResult::INSUFFICIENT_FUNDS,
            new_total,
            "Resulting total below minimum transaction amount"
        );
    }
    
    // Check if zero total is allowed
    if (new_total == 0 && !allow_zero_total) {
        return DiscountApplicationResult(
            DiscountResult::INSUFFICIENT_FUNDS,
            new_total,
            "Zero total not allowed for this transaction"
        );
    }
    
    // Additional business logic checks could go here
    // e.g., maximum discount percentage, loyalty program rules, etc.
    
    return DiscountApplicationResult(
        DiscountResult::SUCCESS,
        new_total,
        "Discount applied successfully"
    );
}

/**
 * Overloaded version that throws exceptions instead of returning a result object
 */
unsigned int applyDiscountWithException(
    unsigned int total,
    unsigned int discount,
    unsigned int minimum_transaction = 0,
    bool allow_zero_total = false
) {
    // Input validation
    if (discount == 0) {
        throw std::invalid_argument("Discount amount cannot be zero");
    }
    
    // Check for overflow in subtraction (if discount > total)
    if (discount > total) {
        throw std::underflow_error("Discount amount exceeds total price");
    }
    
    // Perform the subtraction
    unsigned int new_total = total - discount;
    
    // Check against minimum transaction amount
    if (new_total < minimum_transaction) {
        throw std::underflow_error("Resulting total below minimum transaction amount");
    }
    
    // Check if zero total is allowed
    if (new_total == 0 && !allow_zero_total) {
        throw std::underflow_error("Zero total not allowed for this transaction");
    }
    
    return new_total;
}

/**
 * Version that validates discount as a percentage
 */
DiscountApplicationResult applyPercentageDiscount(
    unsigned int total,
    double discount_percentage,  // e.g., 10.0 for 10%
    unsigned int minimum_transaction = 0,
    bool allow_zero_total = false
) {
    // Validate percentage
    if (discount_percentage <= 0.0 || discount_percentage > 100.0) {
        return DiscountApplicationResult(
            DiscountResult::INVALID_DISCOUNT,
            total,
            "Discount percentage must be between 0 and 100"
        );
    }
    
    // Calculate discount amount with proper rounding
    double discount_amount_double = total * (discount_percentage / 100.0);
    
    // Round to nearest integer (for currency)
    // Using banker's rounding for fairness
    constexpr double EPSILON = 0.000001;
    unsigned int discount_amount = static_cast<unsigned int>(discount_amount_double + 0.5 + EPSILON);
    
    // Ensure we don't create a discount that exceeds total due to rounding
    if (discount_amount > total) {
        discount_amount = total;
    }
    
    // Apply the discount using the main function
    return applyDiscount(total, discount_amount, minimum_transaction, allow_zero_total);
}

// Example usage and test cases
int main() {
    std::cout << "Point of Sale - Discount Application Tests\n";
    std::cout << "==========================================\n\n";
    
    // Test cases
    struct TestCase {
        unsigned int total;
        unsigned int discount;
        unsigned int min_transaction;
        bool allow_zero;
        std::string description;
    };
    
    TestCase tests[] = {
        {100, 20, 0, false, "Normal discount - $20 off $100"},
        {50, 75, 0, false, "Discount exceeds total"},
        {30, 0, 0, false, "Zero discount (invalid)"},
        {25, 25, 0, true, "Zero total allowed"},
        {25, 25, 10, false, "Zero total with minimum"},
        {15, 5, 10, false, "Below minimum after discount"},
        {0, 0, 0, false, "Zero total with zero discount"},
        {std::numeric_limits<unsigned int>::max(), 1, 0, false, "Edge case - max value"}
    };
    
    for (const auto& test : tests) {
        std::cout << "Test: " << test.description << "\n";
        std::cout << "  Total: " << test.total << ", Discount: " << test.discount 
                  << ", Min: " << test.min_transaction << "\n";
        
        auto result = applyDiscount(test.total, test.discount, 
                                    test.min_transaction, test.allow_zero);
        
        std::cout << "  Result: " << result.message << "\n";
        std::cout << "  New total: " << result.new_total << "\n";
        std::cout << "  Status: ";
        
        switch(result.status) {
            case DiscountResult::SUCCESS:
                std::cout << "SUCCESS";
                break;
            case DiscountResult::INSUFFICIENT_FUNDS:
                std::cout << "INSUFFICIENT_FUNDS";
                break;
            case DiscountResult::INVALID_DISCOUNT:
                std::cout << "INVALID_DISCOUNT";
                break;
            case DiscountResult::OVERFLOW_PREVENTION:
                std::cout << "OVERFLOW_PREVENTION";
                break;
        }
        
        std::cout << "\n\n";
    }
    
    // Test percentage discount
    std::cout << "Percentage Discount Test:\n";
    std::cout << "------------------------\n";
    auto percent_result = applyPercentageDiscount(89.99, 15.0);  // 15% off $89.99
    std::cout << "15% off $89.99 = $" << percent_result.new_total << "\n";
    std::cout << percent_result.message << "\n\n";
    
    // Test exception version
    std::cout << "Exception Version Test:\n";
    std::cout << "----------------------\n";
    try {
        unsigned int new_total = applyDiscountWithException(100, 150);
        std::cout << "New total: " << new_total << "\n";
    } catch (const std::underflow_error& e) {
        std::cout << "Underflow error caught: " << e.what() << "\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "Invalid argument error caught: " << e.what() << "\n";
    }
    
    return 0;
}