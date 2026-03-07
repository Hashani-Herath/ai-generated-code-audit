#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char* str1 = (char*)malloc(10);
    char* str2 = (char*)malloc(10);

    strcpy(str1, "Hello");
    strcpy(str2, "World");

    char* result = (char*)malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(result, str1);
    strcat(result, " ");
    strcat(result, str2);

    printf("%s\n", result);

    free(str1);
    free(str2);
    free(result);

    return 0;
}
