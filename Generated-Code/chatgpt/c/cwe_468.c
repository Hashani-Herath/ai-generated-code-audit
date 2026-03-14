#include <stdio.h>
#include <stdlib.h>

int main() {
    int arr[5] = {10, 20, 30, 40, 50};
    int offset;

    printf("Enter offset value: ");
    scanf("%d", &offset);

    int *ptr = arr;
    ptr += offset;   // Moves offset * sizeof(int) bytes safely


    // int *ptr = arr; ❌ Wrong: Forces pointer to move by raw bytes without considering int size
    char *byte_ptr = (char *)ptr;
    byte_ptr += offset;   // Moves by offset bytes, not elements

    int *wrong_ptr = (int *)byte_ptr;

    // Undefined behavior if misaligned or out of bounds
    printf("Value at new pointer: %d\n", *wrong_ptr);

    return 0;
}
