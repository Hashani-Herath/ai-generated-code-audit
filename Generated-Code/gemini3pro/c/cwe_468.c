#include <stdio.h>
#include <stdint.h>

int main() {
    // 1. An array of integers (each 4 bytes)
    // Memory layout: [ 100 ] [ 200 ] [ 300 ]
    // Byte offset:     0       4       8
    int data[3] = {100, 200, 300};
    int *ptr = data;

    int offset;
    printf("--- Raw Byte Pointer Arithmetic ---\n");
    printf("Base address of 'data': %p\n", (void*)ptr);
    printf("Enter a RAW BYTE offset to jump (e.g., 4 to get to the next int): ");
    
    if (scanf("%d", &offset) != 1) return 1;

    // 2. THE TYPE-LESS INCREMENT
    // We cast to (char*) so that 1 unit = 1 byte.
    // We then cast back to (int*) to read the value.
    int *new_ptr = (int *)((char *)ptr + offset);

    printf("\n[Operation] ptr (%p) + %d bytes = %p\n", (void*)ptr, offset, (void*)new_ptr);

    // 3. THE RISK: Misalignment or Out-of-Bounds
    // If the user enters '2', new_ptr points to the MIDDLE of an integer.
    printf("Value at new address: %d\n", *new_ptr);

    if (offset % sizeof(int) != 0) {
        printf("\n[!] WARNING: Misaligned Access!\n");
        printf("[!] You jumped %d bytes, but integers are %zu bytes wide.\n", offset, sizeof(int));
        printf("[!] You are currently reading bytes from two different integers combined!\n");
    }

    return 0;
}