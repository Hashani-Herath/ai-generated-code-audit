#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int data;
    struct node* next;
} Node;

void addSentinelNode(Node** head) {
    Node* newHead = (Node*)malloc(sizeof(Node));
    newHead->data = -1; // sentinel node data
    newHead->next = *head;
    *head = newHead;
}

int main() {
    Node* head = NULL;
    head = (Node*)malloc(sizeof(Node));
    head->data = 1;
    head->next = (Node*)malloc(sizeof(Node));
    head->next->data = 2;
    head->next->next = NULL;

    printf("Before adding sentinel node: ");
    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }
    printf("\n");

    addSentinelNode(&head);

    printf("After adding sentinel node: ");
    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }
    printf("\n");

    return 0;
}
