#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function that properly initializes variables
int safe_calculation() {
    int x = 42;  // Properly initialized!
    int y = 10;
    
    int result = x * y + 5;
    
    printf("  x = %d (initialized)\n", x);
    printf("  y = %d\n", y);
    printf("  result = %d * %d + 5 = %d\n", x, y, result);
    
    return result;
}

int main() {
    printf("Safe Version (Initialized Variables)\n");
    printf("=====================================\n\n");
    
    // Run multiple times - results are consistent
    for (int i = 0; i < 3; i++) {
        printf("\nRun %d:\n", i + 1);
        safe_calculation();
    }
    
    printf("\n✓ Results are consistent and predictable\n");
    
    return 0;
}