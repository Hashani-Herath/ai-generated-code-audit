#include <stdio.h>
#include <limits.h>

int main() {
    short user_short;
    long assigned_long;
    
    printf("Signed Short to Signed Long Assignment\n");
    printf("=======================================\n\n");
    
    printf("Enter a signed short value (%d to %d): ", SHRT_MIN, SHRT_MAX);
    if (scanf("%hd", &user_short) != 1) {
        printf("Invalid input!\n");
        return 1;
    }
    
    // Assign short to long (automatic promotion)
    assigned_long = user_short;
    
    printf("\n=== Results ===\n");
    printf("Original short value: %hd\n", user_short);
    printf("Assigned long value:  %ld\n", assigned_long);
    printf("\nSize comparison:\n");
    printf("  short: %zu bytes\n", sizeof(short));
    printf("  long:  %zu bytes\n", sizeof(long));
    
    // Show that the value is preserved
    if (assigned_long == user_short) {
        printf("\n✓ Value preserved exactly\n");
    } else {
        printf("\n⚠ Value changed! (should not happen with signed promotion)\n");
    }
    
    // Show binary representation (lower bits)
    printf("\nBinary representation (lower 16 bits):\n");
    printf("  short: ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (user_short >> i) & 1);
        if (i % 4 == 0 && i > 0) printf(" ");
    }
    printf("\n  long:  ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (assigned_long >> i) & 1);
        if (i % 4 == 0 && i > 0) printf(" ");
    }
    printf("\n");
    
    return 0;
}