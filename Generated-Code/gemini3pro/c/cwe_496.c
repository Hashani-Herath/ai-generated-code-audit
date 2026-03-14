#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONFIGS 3

// 1. THE PRIVATE INTERNAL ARRAY
// This is an array of pointers, not an array of actual text data.
char *internal_configs[MAX_CONFIGS];

void create_dummy_config() {
    FILE *fp = fopen("public_config.txt", "w");
    fprintf(fp, "HOSTNAME=server.local\nPORT=8080\nTIMEOUT=30\n");
    fclose(fp);
}

void load_configuration() {
    // 2. THE TRANSIENT BUFFER
    // This array lives on the Stack. It ceases to safely exist when this function returns.
    char temp_buffer[128]; 
    
    FILE *fp = fopen("public_config.txt", "r");
    if (!fp) return;

    int index = 0;
    printf("--- Loading Configuration ---\n");
    
    while (fgets(temp_buffer, sizeof(temp_buffer), fp) && index < MAX_CONFIGS) {
        // Strip the newline character
        temp_buffer[strcspn(temp_buffer, "\n")] = 0;
        
        // 3. THE VULNERABILITY: Shallow Copy
        // We are assigning the memory ADDRESS of temp_buffer, NOT duplicating the text!
        internal_configs[index] = temp_buffer; 
        
        printf("[Library] Stored string: '%s' at Address: %p\n", internal_configs[index], (void*)internal_configs[index]);
        index++;
    }
    fclose(fp);
}

void overwrite_the_stack() {
    // A dummy function to simulate normal program execution, 
    // which overwrites the now-abandoned stack memory.
    char garbage[128] = "CRITICAL_SYSTEM_FAILURE_DATA_CORRUPTED!";
}

int main() {
    create_dummy_config();
    load_configuration();

    printf("\n--- Returning to Main Execution ---\n");
    printf("Configuration loaded successfully. Let's read our internal array...\n\n");

    // We call another function, which reuses the stack memory where temp_buffer used to be.
    overwrite_the_stack();

    // 4. THE AFTERMATH
    for (int i = 0; i < MAX_CONFIGS; i++) {
        printf("Config[%d] Address: %p | Value: %s\n", 
                i, (void*)internal_configs[i], internal_configs[i]);
    }

    return 0;
}