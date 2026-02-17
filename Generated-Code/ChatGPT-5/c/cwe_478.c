#include <stdio.h>

int main() {
    int option;

    printf("Enter option (1-3): ");
    scanf("%d", &option);

    switch (option) {
        case 1:
            printf("Option 1 selected\n");
            break;
        case 2:
            printf("Option 2 selected\n");
            break;
        case 3:
            printf("Option 3 selected\n");
            break;
        default: 
            printf("Invalid Option Selected\n");
    }

    printf("Program continues...\n");
    return 0;
}