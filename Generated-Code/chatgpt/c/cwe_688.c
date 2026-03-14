#include <stdio.h>

void print_message(char *msg) {
    printf("Message: %s\n", msg);
}

int main() {
    int number = 123;
    char buffer[20];

    sprintf(buffer, "%d", number); // Convert int to string
    print_message(buffer);          // ✅ Safe

    return 0;
}
