#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    printf("--- Insecure Default Permissions (umask) Demonstration ---\n");

    // 1. THE VULNERABILITY
    // Setting umask to 0000 removes all default OS restrictions.
    // The process is telling the OS: "Do not mask out any permissions."
    mode_t old_mask = umask(0000); 
    
    printf("[Setup] System umask changed from %04o to 0000.\n", old_mask);
    printf("[Setup] ALL files created by this process will now be world-writable!\n\n");

    // 2. THE CONSEQUENCE
    // We create a "secure" file. The open() call requests 0666 (rw-rw-rw-).
    // Normally, the OS would apply the default umask (like 0022) to downgrade this to 0644.
    // Because our mask is 0000, the file gets the full, dangerous 0666 permissions.
    int fd = open("app_secrets.conf", O_CREAT | O_WRONLY | O_TRUNC, 0666);

    if (fd == -1) {
        perror("Failed to create file");
        return 1;
    }

    // Write some sensitive data
    const char *secret = "DB_PASSWORD=SuperSecretAdmin123\n";
    write(fd, secret, 32);
    close(fd);

    printf("[!] Created 'app_secrets.conf'.\n");
    printf("[!] If you run 'ls -l app_secrets.conf' in your terminal, you will see:\n");
    printf("    -rw-rw-rw- 1 user group 32 Feb 17 12:00 app_secrets.conf\n\n");
    
    printf("[CRITICAL] Any other user on this system can now read your password.\n");
    printf("[CRITICAL] Worse, they can OVERWRITE it to point to their own malicious database.\n");

    return 0;
}