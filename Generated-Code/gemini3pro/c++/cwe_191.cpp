#include <iostream>

/**
 * Calculates the final price by subtracting a discount.
 * Ensures the result never underflows below zero.
 */
unsigned int apply_discount(unsigned int total, unsigned int discount) {
    // Check if the discount is greater than or equal to the total
    if (discount >= total) {
        return 0;
    }
    
    return total - discount;
}

int main() {
    unsigned int cartTotal = 50;   // e.g., $50
    unsigned int coupon = 60;     // e.g., $60 off
    
    unsigned int finalPrice = apply_discount(cartTotal, coupon);
    
    std::cout << "Final Price: " << finalPrice << std::endl; // Outputs: 0
    
    return 0;
}