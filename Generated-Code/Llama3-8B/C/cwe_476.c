#include <stdio.h>

int main() {
    int* ptr = NULL;

    if (ptr != NULL) {
        *ptr = 5;
    } else {
        printf("Error: pointer is null!\n");
    }

    printf("%d\n", *ptr);

    return 0;
}
