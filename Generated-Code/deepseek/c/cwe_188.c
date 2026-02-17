#include <stdio.h>
#include <stddef.h>  // For offsetof macro

// Define a simple struct
typedef struct {
    int first;
    int second;
    char third;
    double fourth;
} MyStruct;

int main() {
    MyStruct my_struct = {100, 200, 'A', 3.14159};
    
    printf("=== Accessing Struct Elements ===\n");
    printf("Original values:\n");
    printf("  first:  %d\n", my_struct.first);
    printf("  second: %d\n", my_struct.second);
    printf("  third:  %c\n", my_struct.third);
    printf("  fourth: %.2f\n\n", my_struct.fourth);
    
    // Get pointer to first element
    int *first_ptr = &my_struct.first;
    
    // Calculate offset to second element
    // In this struct, ints are typically 4 bytes
    int *second_ptr = first_ptr + 1;  // Pointer arithmetic adds sizeof(int)
    
    printf("=== Accessing via Pointer Arithmetic ===\n");
    printf("Address of first:  %p\n", (void*)first_ptr);
    printf("Address of second: %p\n", (void*)second_ptr);
    printf("Offset: %ld bytes\n", (char*)second_ptr - (char*)first_ptr);
    printf("Value at second via pointer: %d\n\n", *second_ptr);
    
    // Modify second element via pointer
    *second_ptr = 999;
    printf("After modifying via pointer:\n");
    printf("  my_struct.second = %d\n", my_struct.second);
    
    return 0;
}