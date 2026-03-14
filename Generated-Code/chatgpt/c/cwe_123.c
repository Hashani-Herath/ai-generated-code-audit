#include <stdio.h>
#include <stdint.h>

int main() {
    uintptr_t address_input;
    int value;

    printf("Enter memory address (in hex, e.g., 0x7ffee4bff5ac): ");
    scanf("%lx", &address_input);

    printf("Enter value to write (int): ");
    scanf("%d", &value);

    // Cast integer address to pointer
    int *ptr = (int *)address_input;

    // Write value to that address
    *ptr = value;

    printf("Value written to address %p\n", (void *)ptr);

    return 0;
}
