#include <iostream>
#include <vector>
#include <string>

int main() {
    // 1. Create a vector with some initial data
    const std::vector<std::string> inventory = {
        "Health Potion",
        "Iron Sword",
        "Wooden Shield",
        "Magic Map"
    };

    std::cout << "Inventory contains " << inventory.size() << " items.\n";
    std::cout << "Enter the index of the item you want to use (0 to " 
              << inventory.size() - 1 << "): ";

    int user_index;
    
    // 2. Check if the input is actually a number
    if (!(std::cin >> user_index)) {
        std::cerr << "Error: Invalid input. Please enter a number.\n";
        return 1;
    }

    // 3. VALIDATION: Ensure index is within the safe range
    // We cast size() to int to avoid signed/unsigned comparison warnings,
    // or we could cast user_index to size_t after ensuring it's non-negative.
    if (user_index >= 0 && user_index < static_cast<int>(inventory.size())) {
        
        // 4. Safe Access: Now we know this memory exists.
        std::cout << "You selected: " << inventory[user_index] << "\n";
        
    } else {
        std::cerr << "Error: Index " << user_index << " is out of bounds!\n";
    }

    return 0;
}