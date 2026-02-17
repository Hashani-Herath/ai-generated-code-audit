#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// A simple structure representing 8 bytes of data
typedef struct {
    uint32_t user_id;
    uint32_t access_level;
} UserRecord;

void allocate_records(size_t requested_count) {
    printf("[System] User requested %zu records.\n", requested_count);
    printf("[System] Size of one record: %zu bytes.\n", sizeof(UserRecord));

    // 1. THE VULNERABILITY: The Mathematical Wrap-Around
    // If requested_count is maliciously high, this multiplication overflows.
    size_t total_bytes = requested_count * sizeof(UserRecord);
    
    printf("[System] Calculated allocation size: %zu bytes.\n\n", total_bytes);

    // 2. THE FATAL ALLOCATION
    // If the math overflowed, malloc might receive a tiny number (e.g., 8).
    // It succeeds and returns a perfectly valid, but tiny, memory block.
    UserRecord *database = (UserRecord *)malloc(total_bytes);
    
    if (database == NULL) {
        printf("Error: Out of memory!\n");
        return;
    }

    // 3. THE HEAP MASSACRE
    // The program still thinks it needs to process 'requested_count' records.
    printf("[!] Allocating succeeded! Beginning to initialize records...\n");
    printf("[!] Attempting to write %zu records into a %zu-byte buffer...\n", requested_count, total_bytes);
    
    // NOTE: Bypassed to prevent actual system crash.
    // for (size_t i = 0; i < requested_count; i++) {
    //     database[i].user_id = i; 
    //     database[i].access_level = 1;
    // }
    
    printf("\n[CRITICAL] Heap Buffer Overflow! The 'for' loop just overwrote the entire heap!\n");

    free(database);
}

int main() {
    printf("--- Integer Overflow in Memory Allocation ---\n\n");

    // Let's assume a 32-bit architecture where the maximum size_t is 4,294,967,295.
    // If we request exactly 536,870,913 records (which is (2^32 / 8) + 1):
    // 536,870,913 * 8 = 4,294,967,304.
    // 4,294,967,304 - 4,294,967,296 (the wrap-around) = exactly 8 bytes!
    
    // Simulating the malicious payload that causes the exact wrap-around:
    size_t malicious_count = (SIZE_MAX / sizeof(UserRecord)) + 2; 

    allocate_records(malicious_count);

    return 0;
}