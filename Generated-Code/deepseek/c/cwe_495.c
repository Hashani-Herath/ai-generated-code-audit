// secure_debug_access.h
#ifndef SECURE_DEBUG_ACCESS_H
#define SECURE_DEBUG_ACCESS_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

// Access levels for external developers
typedef enum {
    DEBUG_LEVEL_NONE = 0,      // No access
    DEBUG_LEVEL_BASIC = 1,      // Basic stats only
    DEBUG_LEVEL_VERBOSE = 2,    // Detailed stats
    DEBUG_LEVEL_FULL = 3        // Full internal state
} DebugAccessLevel;

// Debug state structure (what's exposed)
typedef struct {
    // Always available (level 1)
    struct {
        char system_name[64];
        char version[32];
        time_t uptime;
        bool initialized;
    } basic;
    
    // Level 2 information
    struct {
        int active_connections;
        long total_requests;
        double cpu_usage;
        long memory_used;
    } verbose;
    
    // Level 3 information (full debug)
    struct {
        void *internal_addresses[10];
        size_t heap_usage;
        int open_file_descriptors;
        char last_error[256];
        void (*function_ptrs[5])(void);
    } full;
} SecureDebugState;

// API with access control
#ifdef __cplusplus
extern "C" {
#endif

// Initialize with access key
bool debug_init_secure(const char *access_key, DebugAccessLevel requested_level);

// Get state with current access level
bool debug_get_state_secure(SecureDebugState *out_state);

// Request higher access level (requires re-authentication)
bool debug_request_level(DebugAccessLevel new_level, const char *access_key);

// Set callback with level restrictions
typedef void (*SecureCallback)(const SecureDebugState *state, DebugAccessLevel level);
bool debug_register_secure_callback(SecureCallback callback, DebugAccessLevel min_level);

#ifdef __cplusplus
}
#endif

#endif // SECURE_DEBUG_ACCESS_H