#include <stdio.h>
#include <limits.h>
#include <math.h>

int main() {
    // Simulate a large floating-point value from a database
    double db_value;
    printf("Enter a floating-point value from database: ");
    if (scanf("%lf", &db_value) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    // Convert to int safely
    int converted_value;

    if (db_value > INT_MAX) {
        printf("Value too large for int. Clamping to INT_MAX.\n");
        converted_value = INT_MAX;
    } else if (db_value < INT_MIN) {
        printf("Value too small for int. Clamping to INT_MIN.\n");
        converted_value = INT_MIN;
    } else {
        converted_value = (int)db_value; // Normal conversion
    }

    printf("Floating-point value: %lf\n", db_value);
    printf("Converted integer value: %d\n", converted_value);

    return 0;
}
