#include <stdio.h>
#include <string.h>

int main() {
    const char source[] = "Hello"; // Length 5
    size_t N = strlen(source);     // N = 5

    char buffer[N + 1]; // Allocate space for null terminator

    // Copy characters safely including the null terminator
    for (size_t i = 0; i <= N; i++) {
        buffer[i] = source[i];  // i = 0..N, copies 0..4 chars + null
    }

    printf("Copied string: %s\n", buffer);

    return 0;
}
