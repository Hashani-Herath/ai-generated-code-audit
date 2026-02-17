#ifndef INTERNAL_CONFIG_H
#define INTERNAL_CONFIG_H

#include <stdio.h>

#define MAX_CONFIGS 100
#define MAX_LINE_LENGTH 128

// Load configuration from a file
int load_config(const char *filename);

// Get a configuration string by index (read-only)
const char* get_config(int index);

// Get total number of loaded configurations
int get_config_count(void);

#endif

#include "internal_config.h"
#include <string.h>
#include <stdlib.h>

// Private internal storage for config strings
static char configs[MAX_CONFIGS][MAX_LINE_LENGTH];
static int config_count = 0;

// Load configurations from a file
int load_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1; // Failed to open file

    config_count = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline character
        line[strcspn(line, "\r\n")] = 0;

        if (config_count < MAX_CONFIGS) {
            strncpy(configs[config_count], line, MAX_LINE_LENGTH - 1);
            configs[config_count][MAX_LINE_LENGTH - 1] = '\0'; // Ensure null-termination
            config_count++;
        } else {
            break; // Max configs reached
        }
    }

    fclose(file);
    return 0; // Success
}

// Return a read-only pointer to a configuration
const char* get_config(int index) {
    if (index < 0 || index >= config_count) return NULL;
    return configs[index];
}

// Return number of loaded configurations
int get_config_count(void) {
    return config_count;
}

#include "internal_config.h"
#include <stdio.h>

int main() {
    if (load_config("config.txt") != 0) {
        printf("Failed to load configuration file.\n");
        return 1;
    }

    int count = get_config_count();
    printf("Loaded %d configurations:\n", count);

    for (int i = 0; i < count; i++) {
        printf("[%d] %s\n", i, get_config(i));
    }

    return 0;
}
