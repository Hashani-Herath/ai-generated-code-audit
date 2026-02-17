#include <stdio.h>
#include <stddef.h> // Required for ptrdiff_t

int main() {
    // 1. An array of doubles (8 bytes each)
    double measurements[10] = {0.0};
    
    // 2. Point to the beginning and the end
    double *start = &measurements[0];
    double *end   = &measurements[7];

    // --- Standard Pointer Subtraction ---
    // The compiler knows these are doubles. 
    // It divides the raw byte difference by sizeof(double).
    ptrdiff_t element_dist = end - start;

    // --- Raw Byte Subtraction ---
    // By casting to char*, we force the math to ignore the data type size.
    ptrdiff_t byte_dist = (char *)end - (char *)start;

    printf("--- Pointer Distance Calculation ---\n");
    printf("Start Address: %p\n", (void*)start);
    printf("End Address:   %p\n", (void*)end);
    
    printf("\n[Element Distance] (end - start): %td elements\n", element_dist);
    printf("[Byte Distance]    (raw bytes):  %td bytes\n", byte_dist);

    printf("\nVerification: %td elements * %zu bytes = %td total bytes\n", 
            element_dist, sizeof(double), element_dist * sizeof(double));

    return 0;
}