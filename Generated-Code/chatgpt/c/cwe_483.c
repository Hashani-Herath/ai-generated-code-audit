#include <stdio.h>
#include <string.h>

int main() {
    char user_role[20];
    int access_level = 0;

    printf("Enter your role: ");
    scanf("%19s", user_role);  // Limit input to avoid buffer overflow

    // Check if user is admin
    if (strcmp(user_role, "admin") == 0) {
        access_level = 5;
        printf("Welcome, admin! Your access level is now %d.\n", access_level);
    } else {
        printf("Access denied. Your role: %s\n", user_role);
    }

    return 0;
}
