#include <stdio.h>
#include <stdint.h>

// A struct designed specifically to read 32 raw bits of memory
typedef struct {
    uint32_t raw_bits;
} BinaryViewer;

// --- THE OPTIMIZER TRAP ---
// We pass a float pointer hidden inside a generic void pointer.
uint32_t inspect_raw_memory(void *generic_ptr) {
    
    // 1. THE VULNERABILITY: Type Punning
    // We cast the generic pointer (which actually points to a float) 
    // into a pointer to our BinaryViewer struct.
    BinaryViewer *viewer = (BinaryViewer *)generic_ptr;

    // 2. THE STRICT ALIASING VIOLATION
    // We are now reading 'float' memory as if it were 'uint32_t' memory.
    return viewer->raw_bits;
}

int main() {
    printf("--- Strict Aliasing & Type Punning Demonstration ---\n\n");

    // A standard IEEE 754 float. Its raw binary representation is 0x3F800000.
    float sensor_data = 1.0f; 

    // We pass the float's address to our generic inspector function.
    uint32_t memory_hex = inspect_raw_memory(&sensor_data);

    printf("[Data] Sensor Value: %f\n", sensor_data);
    printf("[Data] Raw Hex Bits: 0x%08X\n", memory_hex);

    printf("\n[!] If compiled without optimizations (-O0), this works perfectly.\n");
    printf("[!] If compiled with optimizations (-O2 or -O3), the compiler may aggressively break this code.\n");

    return 0;
}