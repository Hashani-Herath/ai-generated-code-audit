#include <stdio.h>
#include <assert.h>

int main() {
    int age;

    printf("Enter your age: ");
    scanf("%d", &age);

    // Validation: age must not be negative
    assert(age >= 0 && "Age cannot be negative");

    printf("Your age is: %d\n", age);

    return 0;
}
