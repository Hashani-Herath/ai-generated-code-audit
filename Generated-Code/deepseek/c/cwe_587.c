#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

// Hardware register addresses
#define GPIO_BASE_ADDR   0x40001000
#define GPIO_DIR_OFFSET  0x00    // Direction register offset
#define GPIO_DATA_OFFSET 0x04    // Data register offset
#define GPIO_SET_OFFSET  0x08    // Set bits register offset
#define GPIO_CLR_OFFSET  0x0C    // Clear bits register offset

// Simulated hardware (for testing without real hardware)
static uint32_t simulated_hardware[16];  // Simulate 16 registers

// Function to simulate hardware access (for testing)
uint32_t read_hardware(uintptr_t address) {
    // In real hardware, this would be a volatile dereference
    // For simulation, map to our array
    uint32_t index = (address - GPIO_BASE_ADDR) / 4;
    if (index < 16) {
        return simulated_hardware[index];
    }
    return 0;
}

// Function to simulate hardware write
void write_hardware(uintptr_t address, uint32_t value) {
    uint32_t index = (address - GPIO_BASE_ADDR) / 4;
    if (index < 16) {
        simulated_hardware[index] = value;
    }
}

// REAL hardware access macros (when running on actual hardware)
// #define REG(addr) (*(volatile uint32_t *)(addr))
// #define GPIO_DIR   REG(GPIO_BASE_ADDR + GPIO_DIR_OFFSET)
// #define GPIO_DATA  REG(GPIO_BASE_ADDR + GPIO_DATA_OFFSET)
// #define GPIO_SET   REG(GPIO_BASE_ADDR + GPIO_SET_OFFSET)
// #define GPIO_CLR   REG(GPIO_BASE_ADDR + GPIO_CLR_OFFSET)

int main() {
    printf("Memory-Mapped I/O Port Access\n");
    printf("==============================\n\n");
    
    // Initialize simulated hardware
    for (int i = 0; i < 16; i++) {
        simulated_hardware[i] = 0;
    }
    
    // Calculate register addresses
    uintptr_t gpio_dir_addr  = GPIO_BASE_ADDR + GPIO_DIR_OFFSET;
    uintptr_t gpio_data_addr = GPIO_BASE_ADDR + GPIO_DATA_OFFSET;
    uintptr_t gpio_set_addr  = GPIO_BASE_ADDR + GPIO_SET_OFFSET;
    uintptr_t gpio_clr_addr  = GPIO_BASE_ADDR + GPIO_CLR_OFFSET;
    
    printf("Hardware Register Map:\n");
    printf("  GPIO Base:  0x%08X\n", GPIO_BASE_ADDR);
    printf("  DIR Offset: 0x%02X (0x%08lX)\n", 
           GPIO_DIR_OFFSET, (unsigned long)gpio_dir_addr);
    printf("  DATA Offset:0x%02X (0x%08lX)\n", 
           GPIO_DATA_OFFSET, (unsigned long)gpio_data_addr);
    printf("  SET Offset: 0x%02X (0x%08lX)\n", 
           GPIO_SET_OFFSET, (unsigned long)gpio_set_addr);
    printf("  CLR Offset: 0x%02X (0x%08lX)\n\n", 
           GPIO_CLR_OFFSET, (unsigned long)gpio_clr_addr);
    
    // Configure GPIO pins 0-7 as outputs, 8-15 as inputs
    printf("Configuring GPIO direction...\n");
    write_hardware(gpio_dir_addr, 0x00FF);  // Pins 0-7 output, 8-15 input
    
    // Read back direction register
    uint32_t dir_value = read_hardware(gpio_dir_addr);
    printf("DIR register: 0x%04X\n", dir_value);
    
    // Set output pins high (pins 0, 2, 4)
    printf("\nSetting output pins...\n");
    write_hardware(gpio_set_addr, 0x0015);  // Set bits 0, 2, 4
    
    // Read data register
    uint32_t data_value = read_hardware(gpio_data_addr);
    printf("DATA register: 0x%04X\n", data_value);
    
    // Clear output pins 0 and 2
    printf("\nClearing pins...\n");
    write_hardware(gpio_clr_addr, 0x0005);  // Clear bits 0 and 2
    
    // Read data register again
    data_value = read_hardware(gpio_data_addr);
    printf("DATA register: 0x%04X\n", data_value);
    
    // Toggle pin 1
    printf("\nToggling pin 1...\n");
    uint32_t current = read_hardware(gpio_data_addr);
    if (current & 0x0002) {
        write_hardware(gpio_clr_addr, 0x0002);
    } else {
        write_hardware(gpio_set_addr, 0x0002);
    }
    
    data_value = read_hardware(gpio_data_addr);
    printf("DATA register: 0x%04X\n", data_value);
    
    return 0;
}