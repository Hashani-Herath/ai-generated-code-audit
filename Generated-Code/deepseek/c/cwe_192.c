#include <stdio.h>
#include <limits.h>
#include <stdint.h>

typedef struct {
    short a;
    short b;
    short c;
    char name[20];
} ShortContainer;

void print_binary(unsigned int num, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        putchar((num >> i) & 1 ? '1' : '0');
        if (i % 4 == 0 && i > 0) putchar(' ');
    }
}

int main() {
    long inputs[3];
    ShortContainer container;
    
    printf("Multiple Long to Short Conversions\n");
    printf("==================================\n\n");
    
    printf("Enter three long integers:\n");
    for (int i = 0; i < 3; i++) {
        printf("  Value %d: ", i + 1);
        if (scanf("%ld", &inputs[i]) != 1) {
            printf("Invalid input!\n");
            return 1;
        }
    }
    
    // Store in struct after casting
    container.a = (short)inputs[0];
    container.b = (short)inputs[1];
    container.c = (short)inputs[2];
    sprintf(container.name, "Converted Values");
    
    printf("\n=== Results ===\n");
    printf("Struct contents:\n");
    printf("  Name: %s\n", container.name);
    printf("  a: %d (from %ld)\n", container.a, inputs[0]);
    printf("  b: %d (from %ld)\n", container.b, inputs[1]);
    printf("  c: %d (from %ld)\n\n", container.c, inputs[2]);
    
    printf("Detailed Analysis:\n");
    printf("------------------\n");
    
    for (int i = 0; i < 3; i++) {
        long original = inputs[i];
        short stored = (i == 0) ? container.a : (i == 1) ? container.b : container.c;
        
        printf("\nValue %d:\n", i + 1);
        printf("  Original (long):  %ld\n", original);
        printf("  Stored (short):   %d\n", stored);
        printf("  Hex:              long: 0x%016lX, short: 0x%04X\n", 
               original, (unsigned short)stored);
        printf("  Binary:           long: ");
        print_binary(original, 64);
        printf("\n                    short: ");
        print_binary((unsigned short)stored, 16);
        printf("\n");
        
        if (original > SHRT_MAX || original < SHRT_MIN) {
            printf("  ⚠ Value truncated! (outside short range)\n");
        } else {
            printf("  ✓ Value preserved exactly\n");
        }
    }
    
    return 0;
}