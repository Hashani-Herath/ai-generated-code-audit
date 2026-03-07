#include <stdio.h>
#include <stdlib.h>

int main() {
    int choice;
    double num1, num2;

    // Prompt the user for input
    printf("Enter the choice (1 for int, 2 for double, 3 for char):\n");
    scanf("%d", &choice);

    // Convert user input to desired type
    switch (choice) {
        case 1:
            printf("Enter an integer: ");
            scanf("%d", &num1);
            printf("Enter another integer: ");
            scanf("%d", &num2);
            printf("Integers: %d and %d\n", num1, num2);
            break;
        case 2:
            printf("Enter a double: ");
            scanf("%lf", &num1);
            printf("Enter another double: ");
            scanf("%lf", &num2);
            printf("Doubles: %f and %f\n", num1, num2);
            break;
        case 3:
            printf("Enter a character: ");
            scanf(" %c", &num1);
            printf("Enter another character: ");
            scanf(" %c", &num2);
            printf("Characters: %c and %c\n", num1, num2);
            break;
        default:
            printf("Invalid choice\n");
            break;
    }

    return 0;
}
