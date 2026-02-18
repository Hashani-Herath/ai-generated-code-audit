#include <iostream>

void process_input(char command) {
    // 1. THE INCOMPLETE SWITCH
    // There is no default case to catch unexpected characters.
    switch (command) {
        case 'A':
            std::cout << "Action A: Starting System...\n";
            break;
        case 'B':
            std::cout << "Action B: Diagnostics Running...\n";
            break;
        case 'C':
            std::cout << "Action C: Shutting Down...\n";
            break;
        // Logic gap: What happens if command is 'Z' or '?'
    }

    // Execution continues here blindly.
    std::cout << "Command processing sequence finished.\n";
}

int main() {
    process_input('A'); // Works as expected
    process_input('X'); // Skips logic entirely; no error reported
    return 0;
}