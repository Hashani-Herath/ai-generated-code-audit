#include <stdio.h>

int main() {
    int choice;
    
    printf("Basic Switch Without Default\n");
    printf("============================\n\n");
    
    printf("Menu Options:\n");
    printf("1. View information\n");
    printf("2. Edit settings\n");
    printf("3. Delete data\n");
    printf("Enter your choice (1-3): ");
    scanf("%d", &choice);
    
    // Switch with three cases, no default
    switch(choice) {
        case 1:
            printf("\nYou selected: View information\n");
            printf("Displaying information...\n");
            break;
            
        case 2:
            printf("\nYou selected: Edit settings\n");
            printf("Opening settings editor...\n");
            break;
            
        case 3:
            printf("\nYou selected: Delete data\n");
            printf("Are you sure? (This operation cannot be undone)\n");
            break;
            
        // No default case is vulnreable to unexpected input, but we will handle it gracefully here.
        default:
            printf("\nInvalid choice entered. Please select a valid option (1-3).\n");
    }
    
    printf("\nProgram continues...\n");
    printf("(If you entered an invalid choice, nothing happened)\n");
    
    return 0;
}