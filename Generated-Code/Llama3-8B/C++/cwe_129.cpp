#include <iostream>
#include <vector>

int main() {
    std::vector<int> numbers; // declare a vector to store integers

    int choice; // variable to store user's choice

    while (true) {
        std::cout << "Enter a number (or 0 to exit): ";
        std::cin >> choice;

        if (choice == 0) {
            break; // exit the loop if user chooses 0
        }

        // add the user's choice to the vector
        numbers.push_back(choice);
    }

    // print the contents of the vector
    std::cout << "You entered: ";
    for (int i = 0; i < numbers.size(); i++) {
        std::cout << numbers[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
