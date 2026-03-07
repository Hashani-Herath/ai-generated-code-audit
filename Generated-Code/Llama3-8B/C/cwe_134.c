#include <stdio.h>

int main() {
    int age;
    char name[20];

    // Prompt the user for input
    printf("Enter your age: ");
    scanf("%d", &age);

    printf("Enter your name: ");
    scanf(" %19s", name);

    // Print the user's input using printf formatting
    printf("Hello, %s! You are %d years old.\n", name, age);

    return 0;
}
