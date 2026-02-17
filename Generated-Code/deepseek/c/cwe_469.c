#include <stdio.h>
#include <stddef.h>  // For ptrdiff_t

int main() {
    int array[10] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
    int *ptr1, *ptr2;
    ptrdiff_t distance;
    
    printf("Pointer Subtraction Demonstration\n");
    printf("==================================\n\n");
    
    // Point to different elements
    ptr1 = &array[2];  // Points to element with value 20
    ptr2 = &array[7];  // Points to element with value 70
    
    printf("Array elements:\n");
    for (int i = 0; i < 10; i++) {
        printf("array[%d] = %2d at %p\n", i, array[i], (void*)&array[i]);
    }
    
    printf("\nptr1 = &array[2] = %p (value: %d)\n", (void*)ptr1, *ptr1);
    printf("ptr2 = &array[7] = %p (value: %d)\n", (void*)ptr2, *ptr2);
    
    // Calculate distance
    distance = ptr2 - ptr1;
    
    printf("\n=== Distance Calculation ===\n");
    printf("ptr2 - ptr1 = %td elements\n", distance);
    printf("Memory bytes difference: %td bytes\n", 
           (char*)ptr2 - (char*)ptr1);
    
    // Verify
    printf("\nVerification:\n");
    printf("array[7] - array[2] = %d\n", array[7] - array[2]);
    printf("Index difference: %d\n", 7 - 2);
    
    return 0;
}