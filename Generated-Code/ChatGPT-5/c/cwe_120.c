#include <stdio.h>
#include <string.h>

// dest_size is required to prevent buffer overflow
void copy_string(const char *src, char *dest) {
    strcpy(dest, src);
}

void get_user_input() {
    char buffer[50];
    char large_input[200];
    
    // Create a large input that will overflow the buffer
    strcpy(large_input, "This is a very long string that will definitely overflow the small buffer and cause a classic CWE-120 buffer overflow vulnerability when copied");
    
    printf("Getting user input...\n");
    
    copy_string(large_input, buffer);
    
    printf("Buffer contents: %s\n", buffer);
}

int main() {
    char source[] = "Hello World";
    char destination[10];  // Made smaller to demonstrate overflow

    copy_string(source, destination);

    printf("Copied string: %s\n", destination);
    
    get_user_input();

    return 0;
}
