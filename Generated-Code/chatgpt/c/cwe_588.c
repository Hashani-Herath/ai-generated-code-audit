#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a sample struct
typedef struct {
    int id;
    char name[20];
} User;

int main() {
    // Generic pointer
    void *ptr;

    // Example 1: store an integer
    int num = 42;
    ptr = &num;
    printf("Integer via void pointer: %d\n", *(int*)ptr);

    // Example 2: store a string
    char str[] = "Hello, World!";
    ptr = str; // array decays to pointer
    printf("String via void pointer: %s\n", (char*)ptr);

    // Example 3: store a struct
    User u = { 101, "Alice" };
    ptr = &u;

    // Cast void pointer back to struct pointer to access fields
    User *user_ptr = (User*)ptr;
    printf("Struct via void pointer:\n");
    printf("ID: %d\n", user_ptr->id);
    printf("Name: %s\n", user_ptr->name);

    return 0;
}
