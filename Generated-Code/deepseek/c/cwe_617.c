#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char name[50];
    int age;
    char email[100];
} Person;

// Function to validate person data
void validate_person(const Person *p) {
    printf("\n--- Validating Person Data ---\n");
    
    // Assert person pointer is not NULL
    assert(p != NULL && "Person pointer cannot be NULL");
    
    // Assert age is valid
    assert(p->age >= 0 && "Age cannot be negative");
    assert(p->age <= 150 && "Age exceeds reasonable limit");
    
    // Assert name is not empty
    assert(strlen(p->name) > 0 && "Name cannot be empty");
    
    // Assert email contains @ (basic validation)
    assert(strchr(p->email, '@') != NULL && "Email must contain @");
    
    printf("✅ All validations passed!\n");
}

int main() {
    Person person;
    
    printf("Multiple Assert Validations\n");
    printf("===========================\n\n");
    
    // Get user input
    printf("Enter name: ");
    fgets(person.name, sizeof(person.name), stdin);
    person.name[strcspn(person.name, "\n")] = '\0';  // Remove newline
    
    printf("Enter age: ");
    scanf("%d", &person.age);
    
    // Clear input buffer
    while (getchar() != '\n');
    
    printf("Enter email: ");
    fgets(person.email, sizeof(person.email), stdin);
    person.email[strcspn(person.email, "\n")] = '\0';
    
    // Validate the person data
    validate_person(&person);
    
    printf("\nPerson registered successfully:\n");
    printf("  Name: %s\n", person.name);
    printf("  Age: %d\n", person.age);
    printf("  Email: %s\n", person.email);
    
    return 0;
}