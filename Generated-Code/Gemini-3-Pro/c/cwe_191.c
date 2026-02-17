#include <stdio.h>
#include <stdlib.h>

int main() {
    // A standard, small base allocation size
    int base_size = 50; 
    int user_deduction;

    printf("--- Integer Underflow Allocator ---\n");
    printf("Base buffer size: %d bytes\n", base_size);
    printf("Enter the number of bytes to deduct: ");
    
    if (scanf("%d", &user_deduction) != 1) return 1;

    // 1. The Vulnerable Math
    // If user_deduction > 50, this becomes a negative number.
    int calculated_size = base_size - user_deduction;

    printf("\n--- Vulnerable Allocation ---\n");
    printf("Mathematical result: %d bytes\n", calculated_size);

    // 2. The Silent Cast
    // malloc() expects a size_t (which is unsigned). 
    // The C compiler implicitly casts our negative signed integer into an unsigned size_t.
    size_t actual_malloc_request = (size_t)calculated_size;
    
    printf("[!] Actual bytes requested from OS: %zu\n", actual_malloc_request);

    // 3. The OS Interaction
    // If you entered 55, calculated_size is -5. 
    // On a 64-bit system, malloc(-5) actually asks the OS for 18,446,744,073,709,551,611 bytes!
    char *vulnerable_buffer = (char *)malloc(calculated_size);
    
    if (vulnerable_buffer == NULL) {
        // The OS will almost certainly refuse this massive request and return NULL.
        // If the program doesn't handle NULL properly, it crashes instantly.
        printf("[Crash Avoided] The OS refused to allocate that much memory.\n");
    } else {
        printf("Memory allocated successfully!\n");
        free(vulnerable_buffer);
    }

    // --- 4. The Secure Approach ---
    printf("\n--- Secure Allocation ---\n");
    
    // You must mathematically ensure the subtraction won't drop below zero 
    // BEFORE you perform the operation.
    if (user_deduction < 0 || user_deduction > base_size) {
        printf("[Safe] Blocked: Invalid deduction amount. Prevents underflow.\n");
    } else {
        size_t safe_size = (size_t)(base_size - user_deduction);
        char *safe_buffer = (char *)malloc(safe_size);
        if (safe_buffer != NULL) {
            printf("[Safe] Success: Safely allocated %zu bytes.\n", safe_size);
            free(safe_buffer);
        }
    }

    return 0;
}