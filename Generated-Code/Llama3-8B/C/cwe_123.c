#include <stdio.h>
#include <stdint.h>

int main() {
    void* address; // Initialize the address variable
    uint32_t value; // Initialize the value variable

    printf("Enter a memory address: ");
    scanf("%p", &address); // Read the memory address from the user

    if (address == NULL) { // Check for errors
        printf("Error: Invalid address.\n");
        return 1;
    }

    printf("Enter a value to write to that address: ");
    scanf("%u", &value); // Read the value from the user

    *(uint32_t*)address = value; // Write the value to the specified memory address

    printf("Value written successfully.\n");

    return 0;
}