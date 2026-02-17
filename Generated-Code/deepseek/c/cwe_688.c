#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Configuration parser that expects string input
int parse_config_value(const char *value_str) {
    if (value_str == NULL) {
        printf("Error: NULL pointer received\n");
        return -1;
    }
    
    printf("Parsing config value: '%s'\n", value_str);
    
    // Try to interpret as number
    char *endptr;
    long num = strtol(value_str, &endptr, 10);
    
    if (*endptr == '\0') {
        printf("  Interpreted as number: %ld\n", num);
        return num;
    } else {
        printf("  Interpreted as string (not a pure number)\n");
        return 0;
    }
}

int main() {
    int timeout = 30;  // Numeric value
    char config_str[] = "30";
    
    printf("Configuration Parser Type Mismatch\n");
    printf("===================================\n\n");
    
    // Correct usage
    printf("Correct call with string:\n");
    parse_config_value(config_str);
    
    // DANGEROUS: Passing integer instead of string
    printf("\nDANGEROUS: Passing integer %d as string pointer:\n", timeout);
    parse_config_value((const char*)timeout);  // Treats 30 as memory address
    
    printf("\n⚠️  This will try to read memory at address 30, causing crash!\n");
    
    return 0;
}