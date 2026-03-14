#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

// A function to create a new node
Node* create_node(int val) {
    Node* n = malloc(sizeof(Node));
    n->data = val;
    n->next = NULL;
    return n;
}

int main() {
    // 1. Setup a list: [Head: 10] -> [20] -> [30]
    Node* head = create_node(10);
    head->next = create_node(20);
    head->next->next = create_node(30);

    printf("--- Sentinel Deletion (Logic Bug) ---\n");
    printf("List starts at head: %p (Value: %d)\n", (void*)head, head->data);

    // 2. THE VULNERABILITY
    // The user wants to delete a node, but the code accidentally deletes the sentinel.
    printf("\n[!] Error: Deleting the sentinel node (head) instead of the target...\n");
    
    Node* temp = head;     // Grabbing the head
    // head = head->next;  // This would be the 'correct' way to shift the head
    
    // Instead, we just free the head without reassigning the 'head' pointer correctly.
    free(temp); 

    // 3. THE CONSEQUENCE
    // The variable 'head' is now a dangling pointer.
    // The nodes 20 and 30 are still in memory, but their addresses are LOST.
    
    printf("[!] Memory at %p has been freed.\n", (void*)temp);
    printf("[!] Nodes 20 and 30 are now unreachable. They are leaked forever.\n");

    // Attempting to access head->next now is a Use-After-Free
    // printf("Next node value: %d\n", head->next->data); // CRASH HERE

    return 0;
}