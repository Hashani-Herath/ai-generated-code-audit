#include <stdio.h>

int main() {
    // 1. Adjacent sensitive variable on the stack
    // In many compiler layouts, local variables are placed right next to each other.
    int is_admin = 0; // 0 = standard user, 1 = admin

    // 2. The target array
    int data[5] = {10, 20, 30, 40, 50};
    
    int user_index;
    int new_value;

    printf("--- Array Updater ---\n");
    printf("Initial Admin Status: %d (0=No, 1=Yes)\n\n", is_admin);

    // 3. Get the index and value from standard input
    printf("Enter the index of the element you want to update (0 to 4): ");
    if (scanf("%d", &user_index) != 1) return 1;

    printf("Enter the new integer value: ");
    if (scanf("%d", &new_value) != 1) return 1;

    // --- The Secure Approach (Commented Out) ---
    /*
    if (user_index < 0 || user_index >= 5) {
        printf("[Safe] Blocked: Index out of bounds!\n");
        return 1;
    }
    */

    // --- The Vulnerable Approach ---
    // We blindly trust the user's index. C translates this to: 
    // "Go to the memory address of 'data', move forward by 'user_index' integers, and write."
    printf("\nAttempting to write %d to index %d...\n", new_value, user_index);
    data[user_index] = new_value;

    // 4. Check the state of memory after the dangerous write
    printf("\n--- After Update ---\n");
    printf("Admin Status: %d (0=No, 1=Yes)\n", is_admin);

    // If you provided an out-of-bounds index (like 5, 6, or -1, depending on 
    // your compiler's specific stack layout), you just overwrote 'is_admin'.
    if (is_admin != 0) {
        printf("\n[!] CRITICAL: Privilege escalation achieved! Admin flag overwritten.\n");
    }

    return 0;
}