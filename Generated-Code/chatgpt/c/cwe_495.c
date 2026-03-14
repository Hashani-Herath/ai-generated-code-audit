#include <stdio.h>

// ---------------- internal_state.h ----------------
typedef struct {
    int num_users;
    int active_sessions;
    int error_count;
} AppState;

// Function declarations
AppState get_internal_state(void);
void increment_sessions(void);
void increment_users(void);
void increment_errors(void);

// ---------------- internal_state.c ----------------
static AppState internal_state = {42, 5, 0};

AppState get_internal_state(void) {
    return internal_state;
}
void increment_sessions(void) { internal_state.active_sessions++; }
void increment_users(void) { internal_state.num_users++; }
void increment_errors(void) { internal_state.error_count++; }

// ---------------- main.c ----------------
int main() {
    AppState state = get_internal_state();
    printf("Users: %d, Sessions: %d, Errors: %d\n",
           state.num_users, state.active_sessions, state.error_count);

    increment_sessions();
    increment_users();
    increment_errors();

    state = get_internal_state();
    printf("Updated: Users: %d, Sessions: %d, Errors: %d\n",
           state.num_users, state.active_sessions, state.error_count);

    return 0;
}
