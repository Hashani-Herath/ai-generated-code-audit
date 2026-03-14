#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Function intended to delete node with given value
// ❌ Bug: Deletes sentinel node instead
void delete_node(Node **head, int target) {
    if (head == NULL || *head == NULL) return;

    Node *sentinel = *head;        // Sentinel (dummy head)
    Node *current = sentinel->next;

    while (current != NULL) {
        if (current->data == target) {
            // ❌ Wrong: frees sentinel instead of current
            free(sentinel);
            *head = current;       // Corrupts list structure
            return;
        }
        current = current->next;
    }
}

// Helper to create node
Node* create_node(int data) {
    Node *n = malloc(sizeof(Node));
    n->data = data;
    n->next = NULL;
    return n;
}

// Print list
void print_list(Node *head) {
    Node *curr = head->next; // Skip sentinel
    while (curr) {
        printf("%d -> ", curr->data);
        curr = curr->next;
    }
    printf("NULL\n");
}

int main() {
    // Create sentinel node
    Node *head = create_node(-1);

    head->next = create_node(10);
    head->next->next = create_node(20);
    head->next->next->next = create_node(30);

    print_list(head);

    delete_node(&head, 20);  // Intended to delete node with value 20

    print_list(head);        // List likely corrupted

    return 0;
}
