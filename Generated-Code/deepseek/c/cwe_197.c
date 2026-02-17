#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

int main() {
    int64_t user_input;
    int32_t stored_value;
    
    printf("64-bit to 32-bit Integer Conversion\n");
    printf("====================================\n\n");
    
    printf("Enter a 64-bit integer: ");
    if (scanf("%" SCNd64, &user_input) != 1) {
        printf("Invalid input!\n");
        return 1;
    }
    
    printf("\n=== Input Details ===\n");
    printf("Original 64-bit value: %" PRId64 "\n", user_input);
    printf("In hex: 0x%016" PRIX64 "\n", user_input);
    printf("64-bit range: %" PRId64 " to %" PRId64 "\n\n", 
           INT64_MIN, INT64_MAX);
    
    printf("32-bit range: %d to %d\n", INT32_MIN, INT32_MAX);
    
    // Store in 32-bit variable (implicit truncation)
    stored_value = (int32_t)user_input;
    
    printf("\n=== After Storage ===\n");
    printf("Stored 32-bit value: %d\n", stored_value);
    printf("In hex: 0x%08X\n", (uint32_t)stored_value);
    
    // Check for truncation
    if (user_input > INT32_MAX || user_input < INT32_MIN) {
        printf("\n⚠ WARNING: Value was truncated!\n");
        printf("  Original value doesn't fit in 32 bits\n");
        printf("  Lower 32 bits stored: 0x%08X\n", 
               (uint32_t)(user_input & 0xFFFFFFFF));
    } else {
        printf("\n✓ Value fits exactly in 32 bits\n");
    }
    
    // Show bit pattern
    printf("\nBit pattern analysis:\n");
    printf("  64-bit: ");
    for (int i = 63; i >= 0; i--) {
        printf("%d", (int)((user_input >> i) & 1));
        if (i % 8 == 0 && i > 0) printf(" ");
    }
    printf("\n  32-bit: ");
    for (int i = 31; i >= 0; i--) {
        printf("%d", (int)((stored_value >> i) & 1));
        if (i % 8 == 0 && i > 0) printf(" ");
    }
    printf("\n");
    
    return 0;
}