#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#define MAX_FILENAME 256
#define MAX_PATH 1024

typedef struct {
    char name[MAX_FILENAME];
    size_t size;
    time_t modified;
} FileInfo;

void show_progress(const char* operation, int percentage) {
    printf("\r   %s: [", operation);
    int bar_width = 30;
    int pos = bar_width * percentage / 100;
    
    for (int i = 0; i < bar_width; i++) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%%", percentage);
    fflush(stdout);
}

int upload_file(const char* filename) {
    printf("\n📤 UPLOAD: %s\n", filename);
    printf("   • Checking file...\n");
    
    // Simulate file check
    if (strlen(filename) == 0) {
        printf("   ❌ Invalid filename\n");
        return -1;
    }
    
    printf("   • Connecting to server...\n");
    sleep(1);
    
    // Simulate upload progress
    for (int i = 0; i <= 100; i += 10) {
        show_progress("Uploading", i);
        usleep(200000);
    }
    printf("\n   ✅ Upload complete! (%zu bytes transferred)\n", rand() % 1000000);
    return 0;
}

int download_file(const char* filename) {
    printf("\n📥 DOWNLOAD: %s\n", filename);
    
    // Simulate file existence check
    if (rand() % 3 == 0) {  // Simulate occasional error
        printf("   ❌ File not found on server\n");
        return -1;
    }
    
    printf("   • Establishing connection...\n");
    sleep(1);
    
    size_t file_size = rand() % 5000000 + 100000;
    
    for (int i = 0; i <= 100; i += 5) {
        show_progress("Downloading", i);
        usleep(150000);
    }
    printf("\n   ✅ Download complete! (%zu bytes received)\n", file_size);
    return 0;
}

int delete_file(const char* filename) {
    char confirm;
    printf("\n🗑️  DELETE: %s\n", filename);
    printf("   ⚠️  This action cannot be undone!\n");
    printf("   Type 'yes' to confirm: ");
    
    char confirmation[10];
    scanf("%s", confirmation);
    
    if (strcmp(confirmation, "yes") == 0) {
        printf("   • Removing file from server...\n");
        sleep(1);
        printf("   ✅ File deleted successfully!\n");
        return 0;
    } else {
        printf("   ❌ Deletion cancelled.\n");
        return -1;
    }
}

void list_files() {
    printf("\n📋 FILE LISTING:\n");
    printf("   ┌──────┬────────────────────────┬──────────┬─────────────────┐\n");
    printf("   │ #    │ Filename               │ Size     │ Modified        │\n");
    printf("   ├──────┼────────────────────────┼──────────┼─────────────────┤\n");
    printf("   │ 1    │ document.pdf           │ 1.2 MB   │ 2024-01-15 14:30│\n");
    printf("   │ 2    │ image.jpg              │ 3.5 MB   │ 2024-01-16 09:45│\n");
    printf("   │ 3    │ script.sh              │ 45 KB    │ 2024-01-17 11:20│\n");
    printf("   │ 4    │ data.csv                │ 2.1 MB   │ 2024-01-18 16:10│\n");
    printf("   │ 5    │ backup.tar.gz           │ 15.8 MB  │ 2024-01-19 08:00│\n");
    printf("   └──────┴────────────────────────┴──────────┴─────────────────┘\n");
    printf("   Total: 5 files, 22.6 MB\n");
}

void show_stats() {
    printf("\n📊 SERVER STATISTICS:\n");
    printf("   • Total files: 156\n");
    printf("   • Used space: 1.2 GB / 10 GB\n");
    printf("   • Bandwidth today: 45 MB uploaded, 78 MB downloaded\n");
    printf("   • Active connections: 3\n");
}

int main() {
    char command;
    char filename[MAX_FILENAME];
    int running = 1;
    int result;
    
    srand(time(NULL));
    
    printf("\n╔════════════════════════════════╗\n");
    printf("║     ADVANCED CLI FILE TOOL     ║\n");
    printf("╚════════════════════════════════╝\n\n");
    
    printf("Available commands:\n");
    printf("  ┌─────┬───────────────────┐\n");
    printf("  │ u   │ Upload file       │\n");
    printf("  │ d   │ Download file     │\n");
    printf("  │ r   │ Delete file       │\n");
    printf("  │ l   │ List files        │\n");
    printf("  │ s   │ Show statistics   │\n");
    printf("  │ h   │ Help              │\n");
    printf("  │ q   │ Quit              │\n");
    printf("  └─────┴───────────────────┘\n");
    
    while (running) {
        printf("\n[CLI]$ ");
        scanf(" %c", &command);
        command = tolower(command);
        
        // For commands that need a filename
        if (command == 'u' || command == 'd' || command == 'r') {
            scanf("%s", filename);
        }
        
        // Switch statement for command handling
        switch(command) {
            case 'u':  // Upload
                result = upload_file(filename);
                if (result == 0) {
                    printf("   📝 Log: Upload operation completed\n");
                }
                break;
                
            case 'd':  // Download
                result = download_file(filename);
                if (result == 0) {
                    printf("   📝 Log: Download operation completed\n");
                }
                break;
                
            case 'r':  // Remove/Delete
                result = delete_file(filename);
                break;
                
            case 'l':  // List
                list_files();
                break;
                
            case 's':  // Statistics
                show_stats();
                break;
                
            case 'h':  // Help
                printf("\nDetailed Help:\n");
                printf("  u <file>  - Upload a file to the server\n");
                printf("  d <file>  - Download a file from the server\n");
                printf("  r <file>  - Permanently delete a file\n");
                printf("  l         - List all files on the server\n");
                printf("  s         - Show server statistics\n");
                printf("  h         - Display this help message\n");
                printf("  q         - Exit the program\n");
                break;
                
            case 'q':  // Quit
                printf("\n👋 Shutting down CLI tool...\n");
                printf("   Goodbye!\n");
                running = 0;
                break;
                
            default:
                printf("❌ Error: Unknown command '%c'\n", command);
                printf("   Type 'h' for available commands\n");
        }
    }
    
    return 0;
}