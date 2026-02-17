#include <stdio.h>
#include <stdint.h>

// Simulated memory-mapped hardware registers
#define HW_REGISTER_ADDR 0x1000

typedef volatile uint32_t* hw_register_t;

// Function that returns pointer to hardware register
hw_register_t get_hardware_register(uintptr_t address) {
    return (hw_register_t)address;
}

// Function that returns pointer to register field
uint32_t* get_register_field(uint32_t *reg, int bit_offset) {
    return reg;  // Simplified
}

int main() {
    printf("Hardware Register Access Simulation\n");
    printf("====================================\n\n");
    
    // Simulate some memory for registers
    uint32_t simulated_hw[10] = {0};
    
    // Get pointer to "hardware register" and set value immediately
    *get_hardware_register((uintptr_t)&simulated_hw[2]) = 0xFF00FF00;
    
    printf("Simulated hardware register[2] = 0x%08X\n", simulated_hw[2]);
    
    // Set multiple registers in one-liners
    *get_hardware_register((uintptr_t)&simulated_hw[0]) = 0x11111111;
    *get_hardware_register((uintptr_t)&simulated_hw[1]) = 0x22222222;
    *get_hardware_register((uintptr_t)&simulated_hw[3]) = 0x33333333;
    
    printf("\nRegister dump:\n");
    for (int i = 0; i < 5; i++) {
        printf("  reg[%d] = 0x%08X\n", i, simulated_hw[i]);
    }
    
    // Bit manipulation through returned pointer
    printf("\n--- Bit manipulation ---\n");
    uint32_t control_reg = 0;
    
    // Set individual bits through returned pointers
    *get_register_field(&control_reg, 0) |= (1 << 0);  // Set bit 0
    *get_register_field(&control_reg, 5) |= (1 << 5);  // Set bit 5
    
    printf("Control register: 0x%08X (bits 0 and 5 set)\n", control_reg);
    
    return 0;
}