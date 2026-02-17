#include <stdio.h>
#include <stdlib.h>

int main() {
    // 1. Create a valid, writable variable so you have a safe address to test
    int test_variable = 42;
    
    printf("--- Arbitrary Memory Writer ---\n");
    printf("For testing, here is the address of a valid, writable variable.\n");
    // %p prints the memory address of the pointer
    printf("Safe address to use: %p\n", (void *)&test_variable);
    printf("Current value at that address: %d\n\n", test_variable);

    // Variables to hold the user's input
    void *user_address = NULL;
    int user_value;

    // 2. Get the memory address from the user 
    // scanf with %p automatically expects a hexadecimal memory address format
    printf("Enter a memory address (in hex, exactly as printed above): ");
    if (scanf("%p", &user_address) != 1) {
        printf("Invalid address format.\n");
        return 1;
    }

    // 3. Get the integer value to write
    printf("Enter an integer value to write to this address: ");
    if (scanf("%d", &user_value) != 1) {
        printf("Invalid value format.\n");
        return 1;
    }

    // 4. The Dangerous Part: Cast the raw void pointer to an integer pointer
    int *ptr = (int *)user_address;

    printf("\nAttempting to write %d to address %p...\n", user_value, user_address);

    // 5. Dereference the pointer and write the value to memory
    // NOTE: If you entered an address that doesn't belong to this process, 
    // the program will instantly crash with a Segmentation Fault right here.
    *ptr = user_value;

    printf("Success! The memory was updated.\n");
    
    // 6. Verify if we successfully overwrote our test variable
    if (ptr == &test_variable) {
        printf("Verified: test_variable is now %d\n", test_variable);
    }

    return 0;
}