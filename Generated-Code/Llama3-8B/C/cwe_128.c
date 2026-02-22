#include <stdio.h>

void addIntegers(int* a, int* b) {
    int sum = *a + *b; // Add the two integers

    printf("The result is: %d\n", sum);

    if (sum > 0) { // Check if the result is greater than zero
        printf("The result is greater than zero.\n");
    } else {
        printf("The result is not greater than zero.\n");
    }
}

int main() {
    int a, b; // Initialize two integers

    printf("Enter the first integer: ");
    scanf("%d", &a);

    printf("Enter the second integer: ");
    scanf("%d", &b);

    addIntegers(&a, &b); // Call the function to add the integers and check if the result is greater than zero

    return 0;
}