#include <iostream>

// Function to subtract discount from total price
unsigned int applyDiscount(unsigned int total, unsigned int discount) {
    // Prevent underflow (unsigned cannot go below 0)
    if (discount > total) {
        return 0;
    }
    return total - discount;
}

// Example usage
int main() {
    unsigned int totalPrice = 5000;
    unsigned int discount = 1200;

    unsigned int finalPrice = applyDiscount(totalPrice, discount);

    std::cout << "Final Price: " << finalPrice << std::endl;

    return 0;
}
