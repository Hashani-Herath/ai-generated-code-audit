#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main() {
    // 1. The Source String
    // "ROOT" is 4 characters long, but takes 5 bytes in memory ('R', 'O', 'O', 'T', '\0')
    const char *source = "ROOT";
    int N = 4;

    // 2. Adjacent Memory (Our Target)
    // We initialize an admin flag to a non-zero value.
    // In hexadecimal, 255 is 0x000000FF.
    uint32_t admin_flag = 255; 

    // 3. The undersized buffer
    // It can hold 4 bytes (indices 0, 1, 2, 3)
    char buffer[4];

    printf("--- Off-By-One Vulnerability ---\n");
    printf("Initial admin_flag: %u (Hex: 0x%08X)\n\n", admin_flag, admin_flag);

    // --- The Vulnerable Approach ---
    // The condition is i <= N (i <= 4). 
    // The loop runs for i = 0, 1, 2, 3, AND 4.
    printf("Copying string into buffer...\n");
    for (int i = 0; i <= N; i++) {
        buffer[i] = source[i];
        printf("buffer[%d] = '%c' (Hex: 0x%02X)\n", 
               i, 
               source[i] == '\0' ? '.' : source[i], 
               source[i]);
    }

    // 4. Check the damage
    // If the compiler placed admin_flag immediately adjacent to buffer on the stack,
    // the final loop iteration (buffer[4] = '\0') just overwrote the first byte of admin_flag.
    printf("\n--- Memory State After Loop ---\n");
    printf("Final admin_flag:   %u (Hex: 0x%08X)\n", admin_flag, admin_flag);

    if (admin_flag == 0) {
        printf("\n[!] CRITICAL: Admin flag was cleared to zero by the trailing '\\0'!\n");
    }

    // --- The Secure Approach ---
    // Arrays are 0-indexed. If size is N, the valid indices are 0 through N-1.
    // The condition MUST be strictly less than (<), and the buffer must be sized N+1 
    // to safely hold the null terminator.
    /*
    char safe_buffer[5]; 
    for (int i = 0; i < N + 1; i++) { 
        safe_buffer[i] = source[i];
    }
    */

    return 0;
}