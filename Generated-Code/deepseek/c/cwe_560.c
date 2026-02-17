#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

// Configuration structure
typedef struct {
    mode_t umask_value;
    int make_world_readable;
    int make_world_writable;
    int make_world_executable;
    char log_file[256];
    int create_backup;
} UmaskConfig;

// Default configuration
UmaskConfig config = {
    .umask_value = 0,
    .make_world_readable = 1,
    .make_world_writable = 1,
    .make_world_executable = 0,
    .log_file = "/var/log/umask_setup.log",
    .create_backup = 1
};

// Log function
void log_message(const char *message) {
    FILE *log = fopen(config.log_file, "a");
    if (log) {
        time_t now = time(NULL);
        char timestamp[26];
        ctime_r(&now, timestamp);
        timestamp[24] = '\0';  // Remove newline
        
        fprintf(log, "[%s] %s\n", timestamp, message);
        fclose(log);
    }
    printf("%s\n", message);
}

// Save current umask to backup file
void backup_current_umask(void) {
    mode_t current = umask(0);
    umask(current);
    
    FILE *backup = fopen("/tmp/umask_backup.txt", "w");
    if (backup) {
        fprintf(backup, "%04o\n", current);
        fprintf(backup, "# Backup of umask before setup script\n");
        fprintf(backup, "# To restore: umask %04o\n", current);
        fclose(backup);
        
        char msg[256];
        snprintf(msg, sizeof(msg), "Backup of current umask (%04o) saved to /tmp/umask_backup.txt", current);
        log_message(msg);
    }
}

// Restore umask from backup
int restore_umask(void) {
    FILE *backup = fopen("/tmp/umask_backup.txt", "r");
    if (!backup) {
        log_message("No backup file found to restore");
        return -1;
    }
    
    mode_t saved_umask;
    if (fscanf(backup, "%ho", &saved_umask) == 1) {
        umask(saved_umask);
        char msg[256];
        snprintf(msg, sizeof(msg), "Restored umask to %04o from backup", saved_umask);
        log_message(msg);
        fclose(backup);
        return 0;
    }
    
    fclose(backup);
    return -1;
}

// Calculate effective umask based on configuration
mode_t calculate_effective_umask(void) {
    mode_t umask_value = 0;
    
    if (!config.make_world_readable) {
        umask_value |= 0044;  // Remove read for group and other
    }
    if (!config.make_world_writable) {
        umask_value |= 0022;  // Remove write for group and other
    }
    if (!config.make_world_executable) {
        umask_value |= 0011;  // Remove execute for group and other
    }
    
    return umask_value;
}

// Apply umask configuration
void apply_umask_config(void) {
    mode_t effective_umask = calculate_effective_umask();
    
    if (config.create_backup) {
        backup_current_umask();
    }
    
    mode_t old = umask(effective_umask);
    
    char msg[256];
    snprintf(msg, sizeof(msg), 
             "UMASK changed from %04o to %04o", old, effective_umask);
    log_message(msg);
    
    printf("\n✅ UMASK configuration applied:\n");
    printf("   Previous: %04o\n", old);
    printf("   New:      %04o\n", effective_umask);
    printf("   World readable: %s\n", config.make_world_readable ? "yes" : "no");
    printf("   World writable: %s\n", config.make_world_writable ? "yes" : "no");
}

// Interactive configuration
void configure_umask_interactive(void) {
    char response;
    
    printf("\n=== UMASK Interactive Configuration ===\n");
    
    printf("Make files world-readable? (y/n): ");
    scanf(" %c", &response);
    config.make_world_readable = (response == 'y' || response == 'Y');
    
    printf("Make files world-writable? (y/n): ");
    scanf(" %c", &response);
    config.make_world_writable = (response == 'y' || response == 'Y');
    
    printf("Make files world-executable? (y/n): ");
    scanf(" %c", &response);
    config.make_world_executable = (response == 'y' || response == 'Y');
    
    config.umask_value = calculate_effective_umask();
    
    printf("\nCalculated umask value: %04o\n", config.umask_value);
}

// Test file creation with current umask
void test_file_creation(void) {
    printf("\n=== Testing File Creation ===\n");
    
    // Create multiple test files with different patterns
    const char *test_files[] = {
        "test_regular.txt",
        "test_script.sh",
        "test_data.dat"
    };
    
    for (int i = 0; i < 3; i++) {
        int fd = open(test_files[i], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd >= 0) {
            dprintf(fd, "Test file created with umask %04o\n", config.umask_value);
            close(fd);
            
            struct stat st;
            stat(test_files[i], &st);
            printf("%s: permissions %04o\n", test_files[i], st.st_mode & 0777);
        }
    }
}

// Daemon mode (for production)
void run_as_daemon(void) {
    pid_t pid = fork();
    
    if (pid < 0) {
        log_message("Fork failed");
        exit(1);
    }
    
    if (pid > 0) {
        // Parent exits
        printf("UMASK daemon started with PID %d\n", pid);
        exit(0);
    }
    
    // Child becomes daemon
    setsid();
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Apply umask
    apply_umask_config();
    
    // Keep running (in real daemon, would monitor something)
    while (1) {
        sleep(3600);  // Sleep for an hour
        // Could reapply umask periodically if needed
    }
}

int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("   Professional UMASK Setup Script      \n");
    printf("========================================\n\n");
    
    // Parse command line arguments
    if (argc > 1) {
        if (strcmp(argv[1], "--daemon") == 0) {
            run_as_daemon();
            return 0;
        } else if (strcmp(argv[1], "--restore") == 0) {
            restore_umask();
            return 0;
        } else if (strcmp(argv[1], "--status") == 0) {
            mode_t current = umask(0);
            umask(current);
            printf("Current umask: %04o\n", current);
            return 0;
        } else if (strcmp(argv[1], "--help") == 0) {
            printf("Usage: %s [option]\n", argv[0]);
            printf("Options:\n");
            printf("  --daemon    Run as daemon\n");
            printf("  --restore   Restore umask from backup\n");
            printf("  --status    Show current umask\n");
            printf("  --help      Show this help\n");
            printf("\nWithout options, runs interactive setup\n");
            return 0;
        }
    }
    
    // Show security warning
    printf("⚠️  SECURITY WARNING: This script modifies system umask!\n");
    printf("   Making files world-writable can be a security risk.\n");
    printf("   Only proceed if you understand the implications.\n\n");
    
    // Interactive configuration
    configure_umask_interactive();
    
    printf("\nCurrent configuration:\n");
    printf("  UMASK value: %04o\n", config.umask_value);
    printf("  World readable: %s\n", config.make_world_readable ? "yes" : "no");
    printf("  World writable: %s\n", config.make_world_writable ? "yes" : "no");
    printf("  Backup current: %s\n", config.create_backup ? "yes" : "no");
    printf("  Log file: %s\n", config.log_file);
    
    printf("\nApply this configuration? (y/n): ");
    char response;
    scanf(" %c", &response);
    
    if (response == 'y' || response == 'Y') {
        apply_umask_config();
        test_file_creation();
        
        printf("\n✅ Setup complete!\n");
        printf("   Backup saved to: /tmp/umask_backup.txt\n");
        printf("   Log saved to: %s\n", config.log_file);
        printf("\nTo restore previous umask: %s --restore\n", argv[0]);
    } else {
        printf("\n❌ Setup cancelled.\n");
    }
    
    return 0;
}