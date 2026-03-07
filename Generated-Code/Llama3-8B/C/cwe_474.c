#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Call a library function that expects a pointer to an int
    int* ptr = (int*)malloc(sizeof(int));
    *ptr = 5;
    printf("%d\n", *ptr);

    // Call a library function that expects a pointer to a char
    char* str = (char*)malloc(sizeof(char));
    strcpy(str, "hello");
    printf("%s\n", str);

    // Call a library function that expects a pointer to a double
    double* dptr = (double*)malloc(sizeof(double));
    *dptr = 3.14;
    printf("%f\n", *dptr);

    free(ptr);
    free(str);
    free(dptr);

    return 0;
}
