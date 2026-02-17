#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <errno.h>

typedef enum {
    CONV_SUCCESS,
    CONV_OVERFLOW,
    CONV_UNDERFLOW,
    CONV_INVALID
} ConversionStatus;

typedef struct {
    int value;
    ConversionStatus status;
    double original;
    char error_msg[100];
} SafeConversion;

SafeConversion safe_float_to_int(double value) {
    SafeConversion result;
    result.original = value;
    result.status = CONV_SUCCESS;
    
    // Check for NaN or infinity
    if (isnan(value)) {
        result.status = CONV_INVALID;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Value is NaN (Not a Number)");
        result.value = 0;
        return result;
    }
    
    if (isinf(value)) {
        result.status = CONV_INVALID;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Value is infinite");
        result.value = 0;
        return result;
    }
    
    // Range checking
    if (value > INT_MAX) {
        result.status = CONV_OVERFLOW;
        result.value = INT_MAX;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Value exceeds INT_MAX (%d)", INT_MAX);
    } else if (value < INT_MIN) {
        result.status = CONV_UNDERFLOW;
        result.value = INT_MIN;
        snprintf(result.error_msg, sizeof(result.error_msg), 
                 "Value below INT_MIN (%d)", INT_MIN);
    } else {
        result.value = (int)value;
    }
    
    return result;
}

void print_safe_conversion(const SafeConversion *conv) {
    printf("\nConversion Result:\n");
    printf("  Original: %f\n", conv->original);
    printf("  Integer: %d\n", conv->value);
    printf("  Status: ");
    
    switch(conv->status) {
        case CONV_SUCCESS:
            printf("SUCCESS");
            break;
        case CONV_OVERFLOW:
            printf("OVERFLOW - %s", conv->error_msg);
            break;
        case CONV_UNDERFLOW:
            printf("UNDERFLOW - %s", conv->error_msg);
            break;
        case CONV_INVALID:
            printf("INVALID - %s", conv->error_msg);
            break;
    }
    printf("\n");
}

int main() {
    double test_values[] = {
        123.456,
        2.147483647e9,    // Just under INT_MAX
        2.147483648e9,    // Just over INT_MAX
        -2.147483648e9,   // INT_MIN
        -2.147483649e9,   // Below INT_MIN
        1.0/0.0,          // Infinity
        0.0/0.0,          // NaN
        3.14159,
        0.0
    };
    
    int num_tests = sizeof(test_values) / sizeof(test_values[0]);
    
    printf("Safe Floating-Point to Integer Converter\n");
    printf("=========================================\n\n");
    
    printf("Testing %d values:\n", num_tests);
    
    for (int i = 0; i < num_tests; i++) {
        printf("\n--- Test %d ---", i + 1);
        SafeConversion conv = safe_float_to_int(test_values[i]);
        print_safe_conversion(&conv);
    }
    
    return 0;
}