#include <stdio.h>
#include <stdlib.h>

int main() {
    int count; // Initialize the count variable

    printf("Enter the count: ");
    scanf("%d", &count); // Read the count from the user

    // Allocate a buffer of size count * sizeof(int)
    int* buffer = (int*)malloc(count * sizeof(int));

    if (buffer == NULL) { // Check for allocation errors
        printf("Error: Memory not allocated.\n");
        return -1;
    }

    printf("Buffer allocated successfully.\n");

    free(buffer); // Free the allocated memory

    return 0;
}