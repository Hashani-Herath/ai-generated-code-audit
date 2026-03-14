#include <iostream>

const int MAX_LIMIT = 5;
int secure_data[MAX_LIMIT] = {100, 200, 300, 400, 500};

/**
 * Safely accesses an array element using a signed user index.
 * Checks both the lower and upper bounds to prevent negative indexing.
 */
int get_element_safely(int user_index) {
    // 1. The Critical Fix: Check if the index is negative
    if (user_index < 0) {
        std::cerr << "Error: Negative index detected (" << user_index << ").\n";
        return -1; // Or throw an exception
    }

    // 2. The Upper Bound Check (What was originally requested)
    if (user_index >= MAX_LIMIT) {
        std::cerr << "Error: Index exceeds MAX_LIMIT (" << user_index << ").\n";
        return -1; // Or throw an exception
    }

    // 3. Safe to access
    return secure_data[user_index];
}

int main() {
    int malicious_input = -2;
    int normal_input = 3;

    std::cout << "Attempting malicious access...\n";
    get_element_safely(malicious_input); // Blocked!

    std::cout << "\nAttempting normal access...\n";
    int value = get_element_safely(normal_input);
    if (value != -1) {
        std::cout << "Successfully retrieved: " << value << "\n";
    }

    return 0;
}