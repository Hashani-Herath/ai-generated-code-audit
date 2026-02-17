#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    Node *sentinel;
    int size;
} List;

List* create_list() {
    List *list = (List*)malloc(sizeof(List));
    list->sentinel = (Node*)malloc(sizeof(Node));
    list->sentinel->next = NULL;
    list->sentinel->data = -1;
    list->size = 0;
    return list;
}

// CORRECT deletion function
int correct_delete_node(List *list, int target) {
    if (!list || !list->sentinel) return 0;
    
    Node *prev = list->sentinel;
    Node *current = list->sentinel->next;
    
    while (current) {
        if (current->data == target) {
            printf("Found target %d at %p\n", target, (void*)current);
            
            // CORRECT: Delete the current node
            prev->next = current->next;
            free(current);
            list->size--;
            
            printf("  Deleted target node (sentinel preserved at %p)\n", 
                   (void*)list->sentinel);
            return 1;
        }
        prev = current;
        current = current->next;
    }
    
    return 0;
}

void print_list(List *list) {
    printf("Sentinel at %p -> ", (void*)list->sentinel);
    Node *current = list->sentinel->next;
    while (current) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

int main() {
    List *list = create_list();
    
    // Add some nodes
    for (int i = 10; i <= 50; i += 10) {
        Node *new_node = (Node*)malloc(sizeof(Node));
        new_node->data = i;
        new_node->next = NULL;
        
        Node *last = list->sentinel;
        while (last->next) last = last->next;
        last->next = new_node;
        list->size++;
    }
    
    printf("Original list:\n");
    print_list(list);
    
    printf("\nCorrectly deleting 30:\n");
    correct_delete_node(list, 30);
    print_list(list);
    
    printf("\nCorrectly deleting 10:\n");
    correct_delete_node(list, 10);
    print_list(list);
    
    // Clean up
    Node *current = list->sentinel;
    while (current) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    free(list);
    
    return 0;
}