#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node structure
typedef struct Node {
    int data;
    int is_sentinel;
    struct Node *next;
} Node;

// List structure
typedef struct {
    Node *head;
    int size;
    int sentinel_count;
} List;

// Create a new sentinel node
Node* create_sentinel() {
    Node *sentinel = (Node*)malloc(sizeof(Node));
    if (!sentinel) return NULL;
    
    sentinel->data = -1;
    sentinel->is_sentinel = 1;
    sentinel->next = NULL;
    
    return sentinel;
}

// Create a new data node
Node* create_node(int value) {
    Node *node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;
    
    node->data = value;
    node->is_sentinel = 0;
    node->next = NULL;
    
    return node;
}

// Initialize list with one sentinel
List* create_list() {
    List *list = (List*)malloc(sizeof(List));
    if (!list) return NULL;
    
    list->head = create_sentinel();
    if (!list->head) {
        free(list);
        return NULL;
    }
    
    list->size = 0;
    list->sentinel_count = 1;
    
    printf("List created with initial sentinel at %p\n", (void*)list->head);
    return list;
}

// BUG: Adds an extra sentinel on every insert
void buggy_insert(List *list, int value) {
    if (!list) return;
    
    // Create data node
    Node *new_node = create_node(value);
    if (!new_node) return;
    
    // BUG: Also create and insert an extra sentinel
    Node *extra_sentinel = create_sentinel();
    if (!extra_sentinel) {
        free(new_node);
        return;
    }
    
    // Find the end of the list
    Node *current = list->head;
    while (current->next) {
        current = current->next;
    }
    
    // Add the new data node
    current->next = new_node;
    current = new_node;
    
    // BUG: Add extra sentinel after the data node
    current->next = extra_sentinel;
    
    list->size++;
    list->sentinel_count++;
    
    printf("Inserted %d at %p (plus extra sentinel at %p)\n", 
           value, (void*)new_node, (void*)extra_sentinel);
}

// Print list showing all nodes
void print_list(List *list) {
    if (!list || !list->head) return;
    
    printf("\nList (size=%d, sentinels=%d):\n", 
           list->size, list->sentinel_count);
    
    Node *current = list->head;
    int pos = 0;
    
    while (current) {
        printf("  [%d] %p: ", pos, (void*)current);
        
        if (current->is_sentinel) {
            printf("SENTINEL (data=%d)", current->data);
        } else {
            printf("DATA (value=%d)", current->data);
        }
        
        if (current->next) {
            printf(" -> %p", (void*)current->next);
        }
        printf("\n");
        
        current = current->next;
        pos++;
    }
}

// Clean up list
void destroy_list(List *list) {
    if (!list) return;
    
    Node *current = list->head;
    int node_count = 0;
    int sentinel_count = 0;
    
    while (current) {
        Node *temp = current;
        current = current->next;
        
        if (temp->is_sentinel) {
            sentinel_count++;
        }
        node_count++;
        
        printf("Freeing node at %p (%s)\n", 
               (void*)temp, temp->is_sentinel ? "SENTINEL" : "DATA");
        free(temp);
    }
    
    printf("Freed %d total nodes (%d sentinels)\n", 
           node_count, sentinel_count);
    free(list);
}

int main() {
    printf("Extra Sentinel on Every Insert Bug\n");
    printf("===================================\n\n");
    
    // Create list
    List *list = create_list();
    
    // Insert some items - each adds an extra sentinel
    printf("\n--- Inserting items ---\n");
    buggy_insert(list, 10);
    buggy_insert(list, 20);
    buggy_insert(list, 30);
    
    print_list(list);
    
    printf("\n--- Inserting more items ---\n");
    buggy_insert(list, 40);
    buggy_insert(list, 50);
    
    print_list(list);
    
    printf("\n=== Analysis ===\n");
    printf("The list now has %d sentinels but only %d data nodes!\n", 
           list->sentinel_count, list->size);
    printf("Each insert added one data node + one extra sentinel.\n");
    printf("The list structure is:\n");
    printf("  Sentinel (initial) -> Data1 -> Sentinel -> Data2 -> Sentinel -> ...\n");
    
    destroy_list(list);
    
    return 0;
}