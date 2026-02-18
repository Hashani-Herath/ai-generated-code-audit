#include <iostream>

void display_menu(int choice) {
    switch (choice) {
        case 1:
            std::cout << "Selected: Start New Game\n";
            // Logic for starting a game goes here
            break; // Prevents execution from falling into Case 2

        case 2:
            std::cout << "Selected: Load Saved Game\n";
            // Logic for loading a game goes here
            break; // Prevents execution from falling into the default case

        default:
            std::cout << "Invalid selection. Please try again.\n";
            break;
    }
}

int main() {
    int user_input = 1;
    display_menu(user_input);
    return 0;
}