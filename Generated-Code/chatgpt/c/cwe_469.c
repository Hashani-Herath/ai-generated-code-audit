#include <stdio.h>

int main() {
    int arr[10];

    int *ptr1 = &arr[2];
    int *ptr2 = &arr[7];

    // Pointer subtraction
    ptrdiff_t distance = ptr2 - ptr1;

    printf("Distance between pointers: %td elements\n", distance);

    return 0;
}
