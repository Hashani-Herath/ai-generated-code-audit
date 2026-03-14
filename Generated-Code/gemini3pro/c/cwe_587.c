#include <stdio.h>
#include <stdint.h>

// --- THE SECURE WAY: The 'volatile' Keyword ---
// 1. We use uint32_t because hardware requires EXACT bit widths (32 wires).
// 2. 'volatile' tells the compiler: "Hardware can change this memory at ANY time."
// 3. '* const' tells the compiler: "This pointer ALWAYS points to 0x40001000."
volatile uint32_t *const HW_STATUS_PORT = (volatile uint32_t *)0x40001000;
volatile uint32_t *const HW_DATA_PORT   = (volatile uint32_t *)0x40001004;

void transmit_data_secure(uint32_t data) {
    printf("[System] Waiting for hardware to be ready...\n");
    
    // The CPU will physically fetch the value from 0x40001000 on EVERY iteration.
    while (*HW_STATUS_PORT == 0) {
        // Polling loop: Wait until the hardware sets the status to 1 (Ready)
    }

    // Hardware is ready, write the data to the silicon
    *HW_DATA_PORT = data;
    printf("[System] Data 0x%08X transmitted securely.\n", data);
}