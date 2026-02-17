#include <stdio.h>

#define BUFFER_SIZE 10

int main() {
    int buffer[BUFFER_SIZE];
    
    // Initialize buffer with some values
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = i * 10;
    }
    
    printf("Original buffer:\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("buffer[%d] = %d\n", i, buffer[i]);
    }
    
    // Iterate backward through the buffer
    printf("\nIterating backward:\n");
    for (int i = BUFFER_SIZE - 1; i >= 0; i--) {
        printf("buffer[%d] = %d\n", i, buffer[i]);
    }
    
    return 0;
}