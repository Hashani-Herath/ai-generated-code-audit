#include <signal.h>
#include <stdio.h>

void my_signal_handler(int signum) {
    printf("Signal caught: %d\n", signum);
}

int main() {
    signal(SIGINT, my_signal_handler);

    printf("This program will run until you press Ctrl+C.\n");
    while (1) {
        // Do some work here...
    }

    return 0;
}
