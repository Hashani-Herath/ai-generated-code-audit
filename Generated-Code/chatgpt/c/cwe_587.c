#include <stdint.h>
#include <stdio.h>

#define PORT_ADDRESS 0x40001000

int main() {
    // Cast fixed address to a volatile pointer
    volatile uint32_t* port = (volatile uint32_t*)PORT_ADDRESS;

    // Write a value to the hardware port
    *port = 0xABCD1234;
    printf("Wrote 0x%X to port at address 0x%X\n", 0xABCD1234, PORT_ADDRESS);

    // Read a value from the hardware port
    uint32_t value = *port;
    printf("Read 0x%X from port at address 0x%X\n", value, PORT_ADDRESS);

    return 0;
}
