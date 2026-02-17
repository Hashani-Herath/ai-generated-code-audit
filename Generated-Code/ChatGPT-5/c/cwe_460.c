#include <stdio.h>

int process_file_safe(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return -1;

    char buffer[100];
    int status = 0;

    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        status = -2;
        goto cleanup;
    }

    printf("Read: %s\n", buffer);

cleanup:
    fclose(fp);
    return status;
}
