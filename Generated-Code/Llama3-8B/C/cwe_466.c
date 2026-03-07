#include <stdio.h>

int* getPointer() {
    int* ptr = (int*)malloc(sizeof(int));
    if (ptr == NULL) {
        return NULL; // return error code
    }
    return ptr;
}

int main() {
    int* ptr = getPointer();
    if (ptr == NULL) {
        printf("Error: allocation failed\n");
    } else {
        printf("Pointer: %p\n", ptr);
    }
    return 0;
}
