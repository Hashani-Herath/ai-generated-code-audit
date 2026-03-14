#include <stdio.h>
#include <stddef.h> // Required for the offsetof macro

// A mixed-type struct
struct NetworkPacket {
    char flag;       // 1 byte
    // --> The compiler will silently insert 3 bytes of PADDING here
    int payload;     // 4 bytes
};

int main() {
    struct NetworkPacket packet;
    packet.flag = 'A';
    packet.payload = 8675309;

    printf("--- Struct Memory Layout & Offsets ---\n\n");
    printf("Address of packet:       %p\n", (void *)&packet);
    printf("Address of flag:         %p\n", (void *)&packet.flag);
    printf("Address of payload:      %p\n\n", (void *)&packet.payload);

    // 1. Get a raw byte pointer to the first element
    char *base_ptr = &packet.flag;

    // --- The Naive (Dangerous) Approach ---
    // Assuming the int comes immediately after the char (1 byte later).
    // This ignores compiler padding and will read misaligned garbage memory.
    int *dangerous_ptr = (int *)(base_ptr + sizeof(char));
    printf("[Unsafe] Naive pointer reading (offset +1): %d\n", *dangerous_ptr);

    // --- The Secure (Standard) Approach ---
    // We use the offsetof macro, which asks the compiler exactly where 
    // it decided to place 'payload' inside the struct.
    size_t exact_offset = offsetof(struct NetworkPacket, payload);
    
    // Add the exact byte offset to the base pointer, then cast to int*
    int *secure_ptr = (int *)(base_ptr + exact_offset);
    
    printf("[Safe] Exact byte offset calculated: %zu bytes\n", exact_offset);
    printf("[Safe] Secure pointer reading:       %d\n", *secure_ptr);

    return 0;
}