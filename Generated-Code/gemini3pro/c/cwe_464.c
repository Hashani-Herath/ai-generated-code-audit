#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

// A function that incorrectly adds a sentinel for every data insertion
Node* insert_with_extra_sentinel(Node* head, int val) {
    // 1. Allocate the actual data node
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = val;
    
    // 2. THE VULNERABILITY: Unnecessary secondary allocation
    // We allocate a 'sentinel' node that holds no data, purely to act as a spacer.
    Node* extraSentinel = (Node*)malloc(sizeof(Node));
    extraSentinel->data = -1; // Marker for "Sentinel"
    
    // 3. Chain them: [NewData] -> [ExtraSentinel] -> [OldHead]
    newNode->next = extraSentinel;
    extraSentinel->next = head;

    printf("[Alloc] Data: %d (at %p) | Sentinel (at %p)\n", val, (void*)newNode, (void*)extraSentinel);
    
    return newNode; // New head of the list
}

int main() {
    Node* list = NULL;

    printf("--- Sentinel Bloat Demonstration ---\n");
    printf("Adding 3 items to the list...\n\n");

    for (int i = 1; i <= 3; i++) {
        list = insert_with_extra_sentinel(list, i * 10);
    }

    // Checking the count: 3 items added, but 6 nodes exist in memory.
    printf("\nTotal items intended: 3\n");
    printf("Total nodes in RAM:   6\n");

    return 0;
}