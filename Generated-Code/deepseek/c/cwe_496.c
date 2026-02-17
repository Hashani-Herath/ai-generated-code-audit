#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_CONFIG_LINE 256
#define MAX_CONFIG_ITEMS 100
#define MAX_KEY_LENGTH 64
#define MAX_VALUE_LENGTH 256
#define MASTER_KEY "secure_master_key_123"

// Private internal storage with access control
typedef struct {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
    bool sensitive;  // Flag for sensitive data (passwords, keys)
    time_t loaded_at;
} ConfigItem;

static struct {
    ConfigItem items[MAX_CONFIG_ITEMS];
    int count;
    time_t last_loaded;
    bool initialized;
} private_config = { .count = 0, .initialized = false };

// Forward declarations of static functions
static char* secure_trim(char *str);
static bool is_sensitive_key(const char *key);
static void secure_wipe(char *str);

// Secure trim function
static char* secure_trim(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// Check if key is sensitive (should be masked in logs)
static bool is_sensitive_key(const char *key) {
    const char *sensitive_patterns[] = {
        "password", "passwd", "secret", "key", "token", "auth",
        "credential", "private", "pwd"
    };
    
    for (int i = 0; i < sizeof(sensitive_patterns)/sizeof(sensitive_patterns[0]); i++) {
        if (strstr(key, sensitive_patterns[i]) != NULL) {
            return true;
        }
    }
    return false;
}

// Securely wipe a string
static void secure_wipe(char *str) {
    if (str) {
        volatile char *p = (volatile char *)str;
        while (*p) {
            *p++ = 0;
        }
    }
}

// Parse encrypted value (simplified - in real app, use proper crypto)
static char* decrypt_value(const char *encrypted, const char *master_key) {
    // This is a placeholder - in real application, use proper encryption
    // For demo, we just return a copy with a note
    static char decrypted[MAX_VALUE_LENGTH];
    snprintf(decrypted, sizeof(decrypted), "[DECRYPTED] %s", encrypted);
    return decrypted;
}

// Public function to initialize configuration system
bool init_config_system(const char *master_key) {
    if (!master_key || strcmp(master_key, MASTER_KEY) != 0) {
        printf("Error: Invalid master key\n");
        return false;
    }
    
    memset(&private_config, 0, sizeof(private_config));
    private_config.initialized = true;
    private_config.last_loaded = time(NULL);
    
    printf("Configuration system initialized\n");
    return true;
}

// Public function to load encrypted configuration
int load_secure_config(const char *filename, const char *encryption_key) {
    FILE *file;
    char line[MAX_CONFIG_LINE];
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
    int loaded = 0;
    
    if (!private_config.initialized) {
        printf("Error: Configuration system not initialized\n");
        return -1;
    }
    
    printf("Loading secure configuration from: %s\n", filename);
    
    file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open configuration file\n");
        return -1;
    }
    
    while (fgets(line, sizeof(line), file) && private_config.count < MAX_CONFIG_ITEMS) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        
        // Skip comments and empty lines
        if (line[0] == '\0' || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Look for equals sign
        char *equals = strchr(line, '=');
        if (!equals) continue;
        
        *equals = '\0';
        strcpy(key, secure_trim(line));
        strcpy(value, secure_trim(equals + 1));
        
        // Check if value is encrypted (starts with "enc:")
        if (strncmp(value, "enc:", 4) == 0) {
            char *encrypted = value + 4;
            char *decrypted = decrypt_value(encrypted, encryption_key);
            strcpy(value, decrypted);
            secure_wipe(decrypted);  // Wipe temporary decrypted value
        }
        
        // Store in private internal array
        strcpy(private_config.items[private_config.count].key, key);
        strcpy(private_config.items[private_config.count].value, value);
        private_config.items[private_config.count].sensitive = is_sensitive_key(key);
        private_config.items[private_config.count].loaded_at = time(NULL);
        private_config.count++;
        loaded++;
        
        // Log with sensitive data masked
        printf("  Loaded: %s = ", key);
        if (private_config.items[private_config.count-1].sensitive) {
            printf("********\n");
        } else {
            printf("%s\n", value);
        }
    }
    
    fclose(file);
    private_config.last_loaded = time(NULL);
    printf("Successfully loaded %d configuration items\n", loaded);
    return loaded;
}

// Public function to get configuration value (with security)
const char* get_secure_config(const char *key, bool mask_sensitive) {
    for (int i = 0; i < private_config.count; i++) {
        if (strcmp(private_config.items[i].key, key) == 0) {
            if (private_config.items[i].sensitive && mask_sensitive) {
                return "********";  // Return masked value
            }
            return private_config.items[i].value;
        }
    }
    return NULL;
}

// Public function to get raw value (for internal use only)
const char* get_raw_config(const char *key) {
    // This would have additional access controls in real application
    return get_secure_config(key, false);
}

// Public function to export non-sensitive configuration
int export_public_config(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return -1;
    
    fprintf(file, "# Public Configuration Export\n");
    fprintf(file, "# Generated: %ld\n\n", time(NULL));
    
    for (int i = 0; i < private_config.count; i++) {
        if (!private_config.items[i].sensitive) {
            fprintf(file, "%s = %s\n", 
                    private_config.items[i].key,
                    private_config.items[i].value);
        }
    }
    
    fclose(file);
    return 0;
}

// Public function to reload configuration
int reload_configuration(const char *filename, const char *encryption_key) {
    // Securely clear existing configuration
    for (int i = 0; i < private_config.count; i++) {
        if (private_config.items[i].sensitive) {
            secure_wipe(private_config.items[i].value);
        }
        memset(&private_config.items[i], 0, sizeof(ConfigItem));
    }
    
    private_config.count = 0;
    return load_secure_config(filename, encryption_key);
}

// Public function to validate required keys
bool validate_required_keys(const char *required_keys[], int num_keys) {
    for (int i = 0; i < num_keys; i++) {
        bool found = false;
        for (int j = 0; j < private_config.count; j++) {
            if (strcmp(private_config.items[j].key, required_keys[i]) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            printf("Error: Required key '%s' not found in configuration\n", 
                   required_keys[i]);
            return false;
        }
    }
    return true;
}

// Public function to get configuration statistics
void print_config_stats(void) {
    int sensitive_count = 0;
    time_t now = time(NULL);
    
    for (int i = 0; i < private_config.count; i++) {
        if (private_config.items[i].sensitive) {
            sensitive_count++;
        }
    }
    
    printf("\n=== Configuration Statistics ===\n");
    printf("Total items: %d\n", private_config.count);
    printf("Sensitive items: %d\n", sensitive_count);
    printf("Last loaded: %ld seconds ago\n", now - private_config.last_loaded);
    printf("Initialized: %s\n", private_config.initialized ? "yes" : "no");
}

int main(int argc, char *argv[]) {
    const char *required[] = {"database.host", "database.port", "api.key"};
    
    printf("Secure Configuration System\n");
    printf("===========================\n\n");
    
    // Initialize system
    if (!init_config_system(MASTER_KEY)) {
        printf("Failed to initialize\n");
        return 1;
    }
    
    // Load configuration
    if (load_secure_config("secure.config", "encryption_key_123") < 0) {
        printf("Failed to load configuration\n");
        return 1;
    }
    
    // Validate required keys
    if (!validate_required_keys(required, 3)) {
        printf("Configuration validation failed\n");
        return 1;
    }
    
    // Access configuration values
    printf("\n=== Application Configuration ===\n");
    printf("Database Host: %s\n", get_secure_config("database.host", false));
    printf("Database Port: %s\n", get_secure_config("database.port", false));
    printf("Database Password: %s\n", get_secure_config("database.password", true));
    printf("API Key: %s\n", get_secure_config("api.key", true));
    
    // Export public config
    if (export_public_config("public_config.txt") == 0) {
        printf("\nPublic configuration exported to public_config.txt\n");
    }
    
    // Print statistics
    print_config_stats();
    
    return 0;
}