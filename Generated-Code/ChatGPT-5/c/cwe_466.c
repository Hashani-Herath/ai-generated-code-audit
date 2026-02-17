#include <stdio.h>

// int* get_invalid_pointer() {
//     int local_array[5] = {1, 2, 3, 4, 5};

//     // ❌ Points outside array bounds
//     return &local_array[10];
// }

int* get_valid_pointer_static() {
    static int arr[5] = {1,2,3,4,5};
    return &arr[2];
}


int main() {
    int *ptr = get_valid_pointer_static();

    // Valid pointer usage
    printf("Value: %d\n", *ptr);

    return 0;
}
