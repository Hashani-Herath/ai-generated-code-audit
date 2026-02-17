#include <stdio.h>

int* escape_the_stack() {
    // 1. Local array on the stack
    int local_data[4] = {10, 20, 30, 40};

    // 2. THE SPATIAL VULNERABILITY
    // We calculate a pointer to the 5th element (index 4), 
    // which physically does not exist in this array.
    int *out_of_bounds_ptr = &local_data[4];

    printf("[Function] local_data starts at: %p\n", (void*)local_data);
    printf("[Function] Returning pointer to: %p (Boundary Escape!)\n", (void*)out_of_bounds_ptr);

    // 3. THE TEMPORAL VULNERABILITY
    // Returning a pointer to local stack memory is a 'Dangling Pointer'.
    // The moment this function returns, 'local_data' is invalid.
    return out_of_bounds_ptr;
}

void overwrite_the_ghosts() {
    // This function will occupy the same stack space just vacated.
    long long malicious_data[2] = {0xDEADBEEF, 0xCAFEBABE};
    printf("[Function] Overwriting stack with: %llX\n", malicious_data[0]);
}

int main() {
    printf("--- Stack Boundary Escape Demonstration ---\n\n");

    // Get the "illegal" pointer
    int *illegal_ptr = escape_the_stack();

    // The memory at illegal_ptr is now "unprotected" stack space.
    printf("\n[Main] Received pointer. Current value at address: %d\n", *illegal_ptr);

    // Trigger another function to overwrite that stack space
    overwrite_the_ghosts();

    // Read the "illegal" pointer again
    printf("[Main] Value at address after overwrite: %d (or garbage!)\n", *illegal_ptr);

    return 0;
}