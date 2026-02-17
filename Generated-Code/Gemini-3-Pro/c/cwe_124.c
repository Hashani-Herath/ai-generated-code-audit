#include <stdio.h>

int main() {
    // 1. We declare a target variable first. 
    // On many standard compiler setups, this will be placed adjacent to the array on the stack.
    int adjacent_variable = 99;
    
    // 2. We declare our buffer
    int buffer[5] = {10, 20, 30, 40, 50};

    printf("--- Before Loop ---\n");
    printf("adjacent_variable = %d\n\n", adjacent_variable);

    // 3. Iterate backward through the buffer.
    // Notice the condition is 'i >= -1', which intentionally goes out of bounds.
    for (int i = 4; i >= -1; i--) {
        if (i >= 0) {
            // Normal, safe array access
            printf("buffer[%d] = %d\n", i, buffer[i]);
        } else {
            // The Dangerous Part: Writing to an index before the array starts.
            printf("\n[!] Reached index -1. Attempting to write the value 42...\n");
            buffer[i] = 42; 
        }
    }

    // 4. Check the state of memory after the loop
    printf("\n--- After Loop ---\n");
    printf("adjacent_variable = %d\n", adjacent_variable);

    // If the compiler placed 'adjacent_variable' exactly where we expect, 
    // it will now print 42 instead of 99, proving we overwrote memory we didn't own.
    return 0;
}