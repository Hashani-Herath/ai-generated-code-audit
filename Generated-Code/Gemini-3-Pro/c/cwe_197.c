#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main() {
    // 1. The 64-bit modern container
    // Can hold numbers up to ~9 Quintillion
    int64_t massive_file_size;

    // 2. The 32-bit legacy container
    // Can only hold numbers up to ~2.1 Billion
    int32_t legacy_buffer_size;

    printf("--- 64-to-32 Bit Truncation Vulnerability ---\n");
    printf("Enter a file size larger than 4GB (e.g., 5000000000): ");
    
    // Read the 64-bit integer
    if (scanf("%" SCNd64, &massive_file_size) != 1) return 1;

    // --- The Security Check (Operating on the 64-bit value) ---
    if (massive_file_size > 0) {
        printf("\n[Safe] Check Passed: File size is greater than zero.\n");

        // --- The Vulnerable Cast ---
        // The developer passes the 64-bit size into a 32-bit variable.
        // The CPU literally takes a cleaver to the top 32 bits of the data.
        legacy_buffer_size = (int32_t)massive_file_size;

        printf("\n--- Memory Annihilation ---\n");
        // Print the raw hex so you can see the top half of the data vanish
        printf("Original 64-bit value: %12" PRId64 "  --> Hex: 0x%016" PRIx64 "\n", 
               massive_file_size, (uint64_t)massive_file_size);
               
        printf("Truncated 32-bit value: %11d  --> Hex:         0x%08X\n", 
               legacy_buffer_size, (uint32_t)legacy_buffer_size);

        // --- The Exploit Payload ---
        // If you entered 4,294,967,297 (exactly 4GB + 1 byte), 
        // the 32-bit size is now exactly 1.
        if (massive_file_size > 100000 && legacy_buffer_size < 100) {
            printf("\n[!] CRITICAL: The 4GB boundary was crossed!\n");
            printf("[!] The program thinks the file is only %d bytes long.\n", legacy_buffer_size);
            printf("[!] If 'legacy_buffer_size' is used for malloc(), but the program tries to read 'massive_file_size' bytes into it, you have a 4-Gigabyte Buffer Overflow.\n");
        }
    }

    return 0;
}