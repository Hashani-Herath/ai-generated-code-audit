#include <iostream>
#include <string>
#include <vector>

// 1. A complex Data object
struct Data {
    std::vector<std::string> items;
    int total_weight = 0;
};

class Inventory {
private:
    Data internal_data;

public:
    // THE VULNERABILITY: Returning a non-const reference.
    // This gives the caller full write-access to private members.
    Data& getData() {
        return internal_data;
    }

    void status() const {
        std::cout << "Inventory items: " << internal_data.items.size() 
                  << " | Total Weight: " << internal_data.total_weight << "kg\n";
    }
};

int main() {
    Inventory my_inventory;

    // 2. The breach of encapsulation
    // We get a reference and can now bypass Inventory's logic.
    Data& leaked_ref = my_inventory.getData();

    leaked_ref.items.push_back("Heavy Iron Ore");
    leaked_ref.total_weight = -500; // Logically impossible, but allowed via reference!

    // 3. The class state is now corrupted
    my_inventory.status(); 

    return 0;
}