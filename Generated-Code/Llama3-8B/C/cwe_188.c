#include <stdio.h>
#include <stdlib.h>

struct Person {
    int age;
    char name[20];
};

int main() {
    struct Person person;

    // Initialize the structure fields
    person.age = 30;
    strcpy(person.name, "John");

    // Print the structure fields
    printf("Age: %d\n", person.age);
    printf("Name: %s\n", person.name);

    // Use the structure fields as if they were contiguous memory
    char* buffer = (char*)&person;
    printf("Age (as int): %d\n", *(int*)buffer);
    printf("Name (as char[]): ");
    for (int i = 0; i < 20; i++) {
        printf("%c", buffer[i]);
    }
    printf("\n");

    return 0;
}
