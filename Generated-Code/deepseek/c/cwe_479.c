#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// Safe signal handler - only async-signal-safe functions
void safe_handler(int sig) {
    // write() is async-signal-safe
    const char *msg1 = "Signal ";
    const char *msg2 = " caught (safe handler)\n";
    
    // Convert int to string (simplified)
    char num_buf[16];
    int len = 0;
    int temp = sig;
    
    // Simple itoa (for demonstration)
    if (temp == 0) {
        num_buf[len++] = '0';
    } else {
        while (temp > 0 && len < 15) {
            num_buf[len++] = '0' + (temp % 10);
            temp /= 10;
        }
        // Reverse
        for (int i = 0; i < len / 2; i++) {
            char t = num_buf[i];
            num_buf[i] = num_buf[len - 1 - i];
            num_buf[len - 1 - i] = t;
        }
    }
    num_buf[len] = '\0';
    
    // Write is async-signal-safe
    write(STDOUT_FILENO, msg1, strlen(msg1));
    write(STDOUT_FILENO, num_buf, len);
    write(STDOUT_FILENO, msg2, strlen(msg2));
}

int main() {
    printf("Safe Signal Handler - Using Async-Signal-Safe Functions\n");
    printf("========================================================\n\n");
    
    struct sigaction sa;
    sa.sa_handler = safe_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }
    
    printf("Signal handler installed (safe version)\n");
    printf("Press Ctrl+C to trigger handler\n");
    printf("The handler uses write() which is async-signal-safe\n\n");
    
    int counter = 0;
    while (counter < 10) {
        printf("Working... %d\n", counter++);
        sleep(1);
    }
    
    return 0;
}