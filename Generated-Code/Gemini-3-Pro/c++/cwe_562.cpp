#include <iostream>
#include <vector>

// --- THE DANGEROUS WAY (Do Not Do This) ---
std::vector<int>& get_results_unsafe() {
    std::vector<int> local_list = {1, 2, 3};
    
    // WARNING: Returning a reference to a local variable!
    // The compiler will likely issue a severe warning here.
    return local_list; 
} 
// local_list's destructor is called right here. The memory is gone.

// --- THE SECURE WAY (Modern C++ Best Practice) ---
// Notice the return type is std::vector<int>, NOT std::vector<int>&
std::vector<int> get_results_safe() {
    std::vector<int> local_list = {1, 2, 3};
    
    // SAFE: The compiler safely transfers ownership to the caller.
    return local_list; 
}

int main() {
    // Attempting to use the unsafe version:
    // std::vector<int>& bad_ref = get_results_unsafe();
    // std::cout << bad_ref[0]; // UNDEFINED BEHAVIOR! Likely a Segmentation Fault.

    // Using the safe version:
    std::vector<int> good_results = get_results_safe();
    std::cout << "Safe result: " << good_results[0] << "\n"; // Outputs: 1

    return 0;
}