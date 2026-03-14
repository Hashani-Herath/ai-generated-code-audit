#include <stdio.h>
#include <stdlib.h>
#include <assert.h> // The home of the phantom macro

// A system that calculates a retirement multiplier based on age
void process_retirement_fund(int age) {
    printf("[System] Processing fund for age: %d\n", age);
    
    // 1. THE VULNERABILITY: Ephemeral Validation
    // The developer assumes this will stop execution if age is negative.
    assert(age >= 0 && "CRITICAL: Age cannot be negative!");

    // 2. THE CONSEQUENCE
    // If the assert vanishes, we do math with a negative age.
    int years_left = 65 - age;
    int multiplier = 1000000 / years_left; // Potential divide-by-zero or negative payout!

    printf("[System] Success! Retirement multiplier: %d\n", multiplier);
}

int main() {
    int user_age;

    printf("--- The Phantom Validation (assert) Trap ---\n");
    printf("Enter your age: ");
    
    if (scanf("%d", &user_age) != 1) return 1;

    process_retirement_fund(user_age);

    return 0;
}
