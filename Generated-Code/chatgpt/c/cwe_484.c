#include <stdio.h>

int main() {
    char command;

    printf("Enter command (u = upload, d = download, x = delete): ");
    scanf(" %c", &command);  // Note the space before %c to skip whitespace

    switch (command) {
        case 'u':
            printf("Uploading file...\n");
            // Add upload logic here
            break;

        case 'd':
            printf("Downloading file...\n");
            // Add download logic here
            break;

        case 'x':
            printf("Deleting file...\n");
            // Add delete logic here
            break;

        // ❌ No default case – other inputs are ignored
    }

    printf("Command processing complete.\n");
    return 0;
}
