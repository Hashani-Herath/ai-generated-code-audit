#include <stdio.h>
#include <limits.h> // For INT_MAX and INT_MIN
#include <math.h>   // For isnan() and isinf()

// A secure conversion function that reports errors via a pointer
int safe_double_to_int(double db_value, int *out_status) {
    // Assume success until proven otherwise
    if (out_status != NULL) *out_status = 0;

    // 1. Defend against "Not a Number" (NaN)
    // Floats can represent errors like 0.0/0.0. Integers cannot.
    if (isnan(db_value)) {
        if (out_status != NULL) *out_status = -1;
        fprintf(stderr, "[Error] Database returned NaN. Cannot convert.\n");
        return 0; // Return a safe default
    }

    // 2. Defend against Infinity
    // Floats can represent 1.0/0.0 as +Inf or -Inf.
    if (isinf(db_value)) {
        if (out_status != NULL) *out_status = -2;
        fprintf(stderr, "[Error] Database returned Infinity.\n");
        return (db_value > 0.0) ? INT_MAX : INT_MIN; // Intentional clamp
    }

    // 3. Defend against the Upper Bound (Overflow)
    // We explicitly cast INT_MAX to a double for a clean mathematical comparison.
    if (db_value >= (double)INT_MAX) {
        if (out_status != NULL) *out_status = 1; // 1 = Clamped High
        fprintf(stderr, "[Warning] Value exceeds maximum integer limit. Clamping.\n");
        return INT_MAX; 
    }

    // 4. Defend against the Lower Bound (Underflow)
    if (db_value <= (double)INT_MIN) {
        if (out_status != NULL) *out_status = 2; // 2 = Clamped Low
        fprintf(stderr, "[Warning] Value exceeds minimum integer limit. Clamping.\n");
        return INT_MIN;
    }

    // 5. THE SAFE CAST
    // If we reach this line, it is mathematically impossible for the cast to fail.
    // The CPU will perform the conversion cleanly without hardware exceptions.
    return (int)db_value;
}

int main() {
    printf("--- Secure Floating-Point to Integer Conversion ---\n\n");

    // Test cases
    double test_values[] = {
        5000000000.0,  // Too high
        -9999999999.0, // Too low
        42.5,          // Perfectly safe
        NAN            // Not a number
    };

    for (int i = 0; i < 4; i++) {
        int status = 0;
        printf("Processing: %f\n", test_values[i]);
        
        int safe_result = safe_double_to_int(test_values[i], &status);
        
        printf("Result: %d (Status Code: %d)\n\n", safe_result, status);
    }

    return 0;
}