#include <iostream>
#include <string>

// Example Data class
class Data {
public:
    int quantity;
    std::string name;

    Data(int q = 0, const std::string& n = "") : quantity(q), name(n) {}
};

class Inventory {
private:
    Data item;

public:
    // Constructor
    Inventory(int quantity = 0, const std::string& name = "")
        : item(quantity, name) {}

    // Getter returning a non-const reference (unsafe)
    Data& getData() {
        return item; // ❌ exposes internal object
    }

    // Safe alternative (read-only)
    const Data& getDataConst() const {
        return item;
    }
};

int main() {
    Inventory inv(10, "Widget");

    // Unsafe: modifying private member through non-const reference
    Data& d = inv.getData();
    d.quantity += 5;
    d.name = "Super Widget";

    std::cout << "Item: " << d.name << ", Quantity: " << d.quantity << std::endl;

    return 0;
}
