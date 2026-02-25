#include <stdio.h>
#include <string.h>

/**
 * @param source Source string to copy from
 * @param destination Destination buffer
 */
void copy_string(const char *source, char *destination) {
    strcpy(destination, source);
}

void manual_copy_vulnerable(const char *source, char *destination) {
    int i = 0;
    while (source[i] != '\0') {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0';
}

void get_user_input() {
    char small_buffer[20];
    char user_input[200];
    
    printf("Enter your name: ");
    fgets(user_input, sizeof(user_input), stdin);
    
    copy_string(user_input, small_buffer);
    
    printf("Hello, %s\n", small_buffer);
}

int main() {
    char source[] = "This is a very long string that will definitely overflow the destination buffer";
    char destination[10]; 
    
    printf("=== CWE-120 Buffer Overflow Demonstration ===\n\n");
    
    printf("1. Direct strcpy overflow:\n");
    copy_string(source, destination);
    printf("Destination: %s\n\n", destination);
    
    char dest2[15];
    printf("2. Manual copy overflow:\n");
    manual_copy_vulnerable(source, dest2);
    printf("Destination: %s\n\n", dest2);
    
    printf("3. User input overflow test:\n");
    get_user_input();
    
    return 0;
}