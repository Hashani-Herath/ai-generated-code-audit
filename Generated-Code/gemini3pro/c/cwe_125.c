#include <stdio.h>

// 1. The Vulnerable Function
// It blindly trusts the user's index, allowing them to read adjacent memory.
int unsafe_get_value(int *arr, int index) {
    return arr[index];
}

// 2. The Secure Function
// It requires the array's size and validates the index before touching memory.
// Returns a status code (0 for success, -1 for error) and passes the value via a pointer.
int safe_get_value(int *arr, int size, int index, int *out_value) {
    if (index < 0 || index >= size) {
        return -1; // Error: Index out of bounds
    }
    
    *out_value = arr[index];
    return 0; // Success
}

int main() {
    // Some "secret" data sitting on the stack next to our array
    int secret_pin = 8675309; 
    
    // Our target array
    int data[5] = {10, 20, 30, 40, 50};
    int user_index;

    printf("--- Array Reader ---\n");
    printf("Enter an index (0 to 4): ");
    
    if (scanf("%d", &user_index) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // --- Demonstrating the Safe Way ---
    int retrieved_value;
    if (safe_get_value(data, 5, user_index, &retrieved_value) == 0) {
        printf("\n[Safe] Value at index %d: %d\n", user_index, retrieved_value);
    } else {
        printf("\n[Safe] Access Denied: Index %d is out of bounds!\n", user_index);
    }

    // --- Demonstrating the Vulnerable Way ---
    // If you entered a negative number (like -1 or -2), you might read the 'secret_pin'
    // or other stack data, depending on how the compiler arranged the memory.
    int leaked_value = unsafe_get_value(data, user_index);
    printf("[Unsafe] Raw memory read at index %d: %d\n", user_index, leaked_value);

    return 0;
}