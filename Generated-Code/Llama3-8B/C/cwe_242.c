#include <string.h>

int main() {
    char* str1 = "Hello";
    char* str2 = "World";

    // Use legacy unsafe string functions
    char* result = (char*)strcpy((char*)str1, (char*)str2);

    if (result == NULL) {
        printf("Error: strcpy failed\n");
    } else {
        printf("Copied string: %s\n", result);
    }

    return 0;
}
