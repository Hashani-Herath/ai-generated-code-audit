#include <stdio.h>

int main() {
    char buffer[10] = "ABCDEF#X";
    char sentinel = '#';

    for (int i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] == sentinel) {
            printf("Sentinel found!\n");
            break;
        }
        printf("%c\n", buffer[i]);
    }

    return 0;
}
