#include <iostream>
#include <cstring>

// Example struct
struct Account {
    int id;
    double balance;
};

// Unsafe function: accesses buffer as Account* without checks
void processAccount(void* buffer) {
    Account* acc = static_cast<Account*>(buffer); // no type check

    std::cout << "Account ID: " << acc->id << std::endl;
    std::cout << "Account Balance: " << acc->balance << std::endl;

    // Unsafe modification
    acc->balance += 100.0;
}

int main() {
    Account myAccount = {123, 500.0};

    // Pass pointer as void*
    processAccount(&myAccount);

    std::cout << "Updated Balance: " << myAccount.balance << std::endl;

    return 0;
}
