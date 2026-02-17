#include <stdio.h>
#include <limits.h>
#include <stdint.h>

typedef struct {
    char description[50];
    unsigned int u_value;
    int s_value;
    int overflow;
} CastResult;

CastResult perform_cast(unsigned int u_val) {
    CastResult result;
    result.u_value = u_val;
    result.s_value = (int)u_val;
    result.overflow = (u_val > INT_MAX) ? 1 : 0;
    
    snprintf(result.description, sizeof(result.description), 
             "0x%X -> %d", u_val, result.s_value);
    
    return result;
}

int main() {
    unsigned int calculations[5];
    CastResult results[5];
    
    printf("Multiple Unsigned to Signed Casts\n");
    printf("==================================\n\n");
    
    // Perform various calculations
    calculations[0] = 100 + 200;                    // Small value
    calculations[1] = 4000000000U;                  // Large value
    calculations[2] = UINT_MAX;                     // Maximum unsigned
    calculations[3] = 0x80000000;                    // 2^31 (INT_MAX+1)
    calculations[4] = 0xFFFFFFFF;                    // All bits set
    
    printf("Original calculations:\n");
    for (int i = 0; i < 5; i++) {
        printf("  [%d] %u (0x%08X)\n", 
               i, calculations[i], calculations[i]);
    }
    
    // Perform casts
    for (int i = 0; i < 5; i++) {
        results[i] = perform_cast(calculations[i]);
    }
    
    printf("\n=== Cast Results ===\n");
    printf("Index | Unsigned (hex) | Signed (dec) | Overflow | Bits\n");
    printf("------|----------------|--------------|----------|------\n");
    
    for (int i = 0; i < 5; i++) {
        printf("  %2d  |    0x%08X   |   %11d   |    %s    | ", 
               i, 
               results[i].u_value, 
               results[i].s_value,
               results[i].overflow ? "yes" : "no");
        
        // Show first 8 bits as example
        for (int b = 7; b >= 0; b--) {
            printf("%d", (results[i].u_value >> b) & 1);
        }
        printf("\n");
    }
    
    return 0;
}