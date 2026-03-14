#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node* create_node(int data) {
    Node *n = malloc(sizeof(Node));
    if (!n) return NULL;
    n->data = data;
    n->next = NULL;
    return n;
}

// ❌ Bug: Creates a new sentinel node every insert
void insert_with_extra_sentinel(Node **head, int value) {
    // Create new sentinel
    Node *new_sentinel = create_node(-1);  

    // Create real data node
    Node *new_node = create_node(value);

    // Chain: new sentinel -> new node -> old list
    new_sentinel->next = new_node;
    new_node->next = *head;

    // Update head to new sentinel
    *head = new_sentinel;
}

void print_list(Node *head) {
    Node *curr = head;
    while (curr != NULL) {
        printf("[%d] -> ", curr->data);
        curr = curr->next;
    }
    printf("NULL\n");
}

int main() {
    Node *head = NULL;

    insert_with_extra_sentinel(&head, 10);
    insert_with_extra_sentinel(&head, 20);
    insert_with_extra_sentinel(&head, 30);

    print_list(head);

    return 0;
}
