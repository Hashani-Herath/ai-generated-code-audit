#include <stdio.h>
#include <stdlib.h>

// --- THE UTILITY LIBRARY ---
// This function assumes it takes ownership of the pointer.
void process_and_cleanup(char *plugin_data) {
    printf("[Utility] Processing external data: %s\n", plugin_data);
    
    // 1. THE VULNERABILITY: Blind Assumption of Origin
    // The utility assumes 'plugin_data' came from malloc().
    printf("[Utility] Attempting to free buffer at %p...\n", (void*)plugin_data);
    
    // CRASH: The heap manager expects metadata just before this address.
    free(plugin_data); 
    
    printf("[Utility] Cleanup complete. (You will never see this line).\n");
}

// --- THE EXTERNAL CALLER ---
int main() {
    printf("--- The Cross-Domain Free Demonstration ---\n\n");

    // SCENARIO A: The Stack Variable
    // This memory is automatically managed by the CPU's stack pointer.
    char local_buffer[] = "I am a temporary stack variable!";
    
    // SCENARIO B: The Read-Only Data Segment
    // char *ro_string = "I am burned into the executable!";

    printf("[Main] Generating local data at %p\n", (void*)local_buffer);
    printf("[Main] Passing data to utility for processing...\n\n");

    // 2. THE FATAL HANDOFF
    // We pass a stack pointer to a function that calls free().
    process_and_cleanup(local_buffer);

    return 0;
}

// --- SECURE API DESIGN ---
// Secure API Design
void* utility_allocate(size_t size);
void utility_process(void* data);
void utility_free(void* data); 

// The caller is forced to use the utility's specific memory manager
void* my_data = utility_allocate(100);
utility_process(my_data);
utility_free(my_data);