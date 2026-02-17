#include <stdio.h>
#include <stdlib.h>

// This is C code attempting to use C++ delete
// WARNING: This will NOT compile with a C compiler!
// It's shown here for educational purposes only

int main() {
    int *c_style_ptr;
    int *cxx_style_ptr;
    
    printf("Mixing malloc with delete - C code perspective\n");
    printf("===============================================\n\n");
    
    // Allocate with malloc (C style)
    c_style_ptr = (int*)malloc(sizeof(int));
    if (!c_style_ptr) return 1;
    
    *c_style_ptr = 42;
    printf("C-style allocation at %p: %d\n", (void*)c_style_ptr, *c_style_ptr);
    
    // WRONG: Using C++ delete on C-allocated memory
    // This would need to be compiled as C++ to even attempt
    printf("\nAttempting to use C++ delete on C memory...\n");
    printf("delete c_style_ptr;  // THIS IS WRONG!\n");
    
    // Allocate with new (C++ style) - can't do in C
    printf("\nAttempting to use C++ new in C...\n");
    printf("cxx_style_ptr = new int;  // THIS IS WRONG!\n");
    
    // WRONG: Using free on C++ allocated memory
    printf("\nAttempting to use free on C++ memory...\n");
    printf("free(cxx_style_ptr);  // THIS IS WRONG!\n");
    
    // Proper C cleanup
    free(c_style_ptr);
    printf("\nProper C cleanup: free(c_style_ptr)\n");
    
    printf("\n=== CRITICAL WARNING ===\n");
    printf("This code demonstrates why you must NEVER mix malloc/free with new/delete!\n");
    printf("The actual code won't compile in C, and if forced in C++, would cause:\n");
    printf("  - Memory corruption\n");
    printf("  - Undefined behavior\n");
    printf("  - Crashes\n");
    printf("  - Resource leaks\n");
    
    return 0;
}