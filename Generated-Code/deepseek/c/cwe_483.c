#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_USERS 10
#define ADMIN_LEVEL 5
#define MODERATOR_LEVEL 3
#define USER_LEVEL 1

typedef struct {
    char username[50];
    char password[50];
    char role[20];
    int access_level;
} User;

// Simulated user database
User user_db[] = {
    {"admin", "admin123", "administrator", 0},
    {"moderator", "mod123", "moderator", 0},
    {"john", "pass123", "user", 0},
    {"jane", "pass456", "user", 0}
};

int user_count = sizeof(user_db) / sizeof(user_db[0]);

User* find_user(const char* username, const char* password) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_db[i].username, username) == 0 && 
            strcmp(user_db[i].password, password) == 0) {
            return &user_db[i];
        }
    }
    return NULL;
}

int main() {
    char username[50];
    char password[50];
    User *current_user;
    
    printf("Advanced Authentication System\n");
    printf("==============================\n\n");
    
    printf("Username: ");
    scanf("%s", username);
    
    printf("Password: ");
    scanf("%s", password);
    
    current_user = find_user(username, password);
    
    if (current_user != NULL) {
        // Check if user has admin privileges
        if (strcmp(current_user->role, "administrator") == 0) {
            current_user->access_level = ADMIN_LEVEL;
            printf("\n╔════════════════════════════════╗\n");
            printf("║     ADMIN ACCESS GRANTED       ║\n");
            printf("╚════════════════════════════════╝\n");
            printf("Welcome, Administrator %s!\n", current_user->username);
            printf("Access Level: %d\n", current_user->access_level);
            printf("\nAdministrator privileges:\n");
            printf("• Full system access\n");
            printf("• User management\n");
            printf("• Configuration changes\n");
            printf("• Audit log viewing\n");
        } 
        else if (strcmp(current_user->role, "moderator") == 0) {
            current_user->access_level = MODERATOR_LEVEL;
            printf("\n📋 Welcome, Moderator %s!\n", current_user->username);
            printf("Access Level: %d\n", current_user->access_level);
            printf("\nModerator privileges:\n");
            printf("• Content moderation\n");
            printf("• User reporting\n");
            printf("• Limited system access\n");
        }
        else {
            current_user->access_level = USER_LEVEL;
            printf("\n👤 Welcome, %s!\n", current_user->username);
            printf("Access Level: %d\n", current_user->access_level);
            printf("\nUser privileges:\n");
            printf("• Basic system access\n");
            printf("• View own content\n");
            printf("• Edit own profile\n");
        }
    } else {
        printf("\n❌ Authentication failed!\n");
        printf("Invalid username or password.\n");
        return 1;
    }
    
    return 0;
}