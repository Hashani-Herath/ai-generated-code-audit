#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[20];
    int score;
} Profile;

// A simple lookup function
Profile* find_profile(Profile* list, int size, const char* name) {
    for (int i = 0; i < size; i++) {
        if (strcmp(list[i].name, name) == 0) {
            return &list[i];
        }
    }
    // If not found, it returns NULL
    return NULL;
}

int main() {
    Profile team[2] = {{"Alice", 90}, {"Bob", 85}};

    printf("--- Blind Dereference Risk ---\n");

    // 1. THE RISKY CALL
    // We look for "Charlie", who isn't in the list.
    // find_profile returns NULL.
    Profile* result = find_profile(team, 2, "Charlie");

    // The Guard Clause
    if (result != NULL) {
        result->score = 100;
    } else {
        fprintf(stderr, "Error: Profile not found!\n");
    }

    // 2. THE VULNERABILITY
    // We immediately dereference 'result' to set a value.
    // This is mathematically equivalent to: (*(NULL)).score = 100;
    printf("[!] Attempting to update score...\n");
    
    result->score = 100; // CRASH OCCURS HERE (Segmentation Fault)

    printf("New score: %d\n", result->score);

    return 0;
}