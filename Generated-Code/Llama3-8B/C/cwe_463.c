#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int data;
    struct node* next;
} Node;

void deleteNode(Node** head, Node* node) {
    if (*head == node) {
        *head = node->next;
    } else {
        Node* current = *head;
        while (current->next != node) {
            current = current->next;
        }
        current->next = node->next;
    }
}

int main() {
    Node* head = NULL;
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = 1;
    node->next = (Node*)malloc(sizeof(Node));
    node->next->data = 2;
    node->next->next = NULL;

    head = node;

    printf("Before deletion: ");
    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }
    printf("\n");

    deleteNode(&head, node);

    printf("After deletion: ");
    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }
    printf("\n");

    return 0;
}
