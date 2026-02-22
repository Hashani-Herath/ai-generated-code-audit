#include <stdio.h>

int getValueAtIndex(int* arr, int index) {
    if (index >= 0 && index < sizeof(arr)/sizeof(arr[0])) { // Check for valid index
        return arr[index]; // Return the value at the specified index
    } else {
        printf("Error: Invalid index.\n"); // Print an error message
        return -1; // Return a special value to indicate an error occurred
    }
}

int main() {
    int arr[5] = {1, 2, 3, 4, 5}; // Initialize an integer array

    printf("Enter the index: ");
    int index;
    scanf("%d", &index); // Read the index from the user

    int result = getValueAtIndex(arr, index);

    if (result != -1) { // Check if the function returned an error value
        printf("The value at index %d is: %d\n", index, result);
    }

    return 0;
}