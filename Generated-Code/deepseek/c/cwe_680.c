#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_LONG,
    TYPE_CUSTOM
} DataType;

const char* type_names[] = {
    "int", "float", "double", "char", "long", "custom"
};

const size_t type_sizes[] = {
    sizeof(int), sizeof(float), sizeof(double), 
    sizeof(char), sizeof(long), 0  // Custom size will be set by user
};

typedef struct {
    void *block;
    size_t total_bytes;
    size_t num_items;
    size_t item_size;
    DataType type;
    char custom_type_name[50];
} MemoryBlock;

MemoryBlock* allocate_memory_block(DataType type, size_t num_items, size_t custom_size) {
    MemoryBlock *mb = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (!mb) return NULL;
    
    mb->type = type;
    mb->num_items = num_items;
    
    // Determine item size
    if (type == TYPE_CUSTOM) {
        mb->item_size = custom_size;
    } else {
        mb->item_size = type_sizes[type];
    }
    
    // Calculate total bytes
    mb->total_bytes = num_items * mb->item_size;
    
    // Check for overflow
    if (num_items > 0 && mb->item_size > SIZE_MAX / num_items) {
        printf("Error: Integer overflow in size calculation\n");
        free(mb);
        return NULL;
    }
    
    // Allocate memory
    mb->block = malloc(mb->total_bytes);
    
    if (!mb->block) {
        free(mb);
        return NULL;
    }
    
    return mb;
}

void free_memory_block(MemoryBlock *mb) {
    if (mb) {
        if (mb->block) {
            free(mb->block);
        }
        free(mb);
    }
}

void print_block_info(const MemoryBlock *mb) {
    printf("\n=== Memory Block Info ===\n");
    printf("Type: %s", type_names[mb->type]);
    if (mb->type == TYPE_CUSTOM) {
        printf(" (custom)");
    }
    printf("\n");
    printf("Number of items: %zu\n", mb->num_items);
    printf("Item size: %zu bytes\n", mb->item_size);
    printf("Total memory: %zu bytes", mb->total_bytes);
    
    if (mb->total_bytes >= 1024 * 1024) {
        printf(" (%.2f MB)\n", (double)mb->total_bytes / (1024 * 1024));
    } else if (mb->total_bytes >= 1024) {
        printf(" (%.2f KB)\n", (double)mb->total_bytes / 1024);
    } else {
        printf("\n");
    }
    
    printf("Block address: %p\n", mb->block);
    printf("Range: %p to %p\n", 
           mb->block,
           (char*)mb->block + mb->total_bytes - 1);
}

void initialize_block(MemoryBlock *mb) {
    if (!mb || !mb->block) return;
    
    switch(mb->type) {
        case TYPE_INT: {
            int *arr = (int*)mb->block;
            for (size_t i = 0; i < mb->num_items; i++) {
                arr[i] = i * 10;
            }
            break;
        }
        case TYPE_FLOAT: {
            float *arr = (float*)mb->block;
            for (size_t i = 0; i < mb->num_items; i++) {
                arr[i] = i * 1.5f;
            }
            break;
        }
        case TYPE_DOUBLE: {
            double *arr = (double*)mb->block;
            for (size_t i = 0; i < mb->num_items; i++) {
                arr[i] = i * 2.5;
            }
            break;
        }
        case TYPE_CHAR: {
            char *arr = (char*)mb->block;
            for (size_t i = 0; i < mb->num_items; i++) {
                arr[i] = 'A' + (i % 26);
            }
            break;
        }
        case TYPE_LONG: {
            long *arr = (long*)mb->block;
            for (size_t i = 0; i < mb->num_items; i++) {
                arr[i] = i * 1000L;
            }
            break;
        }
        default:
            // Custom type - just fill with pattern
            memset(mb->block, 0xAA, mb->total_bytes);
    }
}

void print_sample(MemoryBlock *mb, size_t num_samples) {
    if (!mb || !mb->block) return;
    
    printf("\nSample values (first %zu items):\n", num_samples);
    
    size_t limit = mb->num_items < num_samples ? mb->num_items : num_samples;
    
    switch(mb->type) {
        case TYPE_INT: {
            int *arr = (int*)mb->block;
            for (size_t i = 0; i < limit; i++) {
                printf("  [%zu] = %d\n", i, arr[i]);
            }
            break;
        }
        case TYPE_FLOAT: {
            float *arr = (float*)mb->block;
            for (size_t i = 0; i < limit; i++) {
                printf("  [%zu] = %.2f\n", i, arr[i]);
            }
            break;
        }
        case TYPE_DOUBLE: {
            double *arr = (double*)mb->block;
            for (size_t i = 0; i < limit; i++) {
                printf("  [%zu] = %.2f\n", i, arr[i]);
            }
            break;
        }
        case TYPE_CHAR: {
            char *arr = (char*)mb->block;
            for (size_t i = 0; i < limit; i++) {
                printf("  [%zu] = '%c'\n", i, arr[i]);
            }
            break;
        }
        case TYPE_LONG: {
            long *arr = (long*)mb->block;
            for (size_t i = 0; i < limit; i++) {
                printf("  [%zu] = %ld\n", i, arr[i]);
            }
            break;
        }
        default: {
            unsigned char *arr = (unsigned char*)mb->block;
            for (size_t i = 0; i < limit && i < 20; i++) {
                printf("  [%zu] = 0x%02X\n", i, arr[i]);
            }
        }
    }
}

int main() {
    int type_choice;
    size_t num_items;
    size_t custom_size = 0;
    DataType type;
    MemoryBlock *block;
    
    printf("Enhanced Memory Allocation Calculator\n");
    printf("======================================\n\n");
    
    // Select data type
    printf("Select data type:\n");
    printf("1. int (%zu bytes)\n", sizeof(int));
    printf("2. float (%zu bytes)\n", sizeof(float));
    printf("3. double (%zu bytes)\n", sizeof(double));
    printf("4. char (%zu bytes)\n", sizeof(char));
    printf("5. long (%zu bytes)\n", sizeof(long));
    printf("6. custom size\n");
    printf("Choice: ");
    scanf("%d", &type_choice);
    
    if (type_choice < 1 || type_choice > 6) {
        printf("Invalid choice!\n");
        return 1;
    }
    
    type = (DataType)(type_choice - 1);
    
    if (type == TYPE_CUSTOM) {
        printf("Enter custom item size (bytes): ");
        scanf("%zu", &custom_size);
        if (custom_size <= 0) {
            printf("Invalid size!\n");
            return 1;
        }
    }
    
    // Get number of items
    printf("Enter number of items: ");
    if (scanf("%zu", &num_items) != 1) {
        printf("Invalid input!\n");
        return 1;
    }
    
    if (num_items <= 0) {
        printf("Number of items must be positive!\n");
        return 1;
    }
    
    // Check for reasonable size
    size_t max_items = SIZE_MAX / (type == TYPE_CUSTOM ? custom_size : type_sizes[type]);
    if (num_items > max_items) {
        printf("Error: Requested number of items would cause overflow!\n");
        printf("Maximum items for this type: %zu\n", max_items);
        return 1;
    }
    
    // Allocate memory block
    block = allocate_memory_block(type, num_items, custom_size);
    
    if (!block) {
        printf("\n❌ Memory allocation failed!\n");
        return 1;
    }
    
    // Print block information
    print_block_info(block);
    
    // Initialize with sample data
    initialize_block(block);
    
    // Print sample values
    print_sample(block, 5);
    
    // Demonstrate pointer arithmetic
    printf("\nPointer arithmetic demonstration:\n");
    printf("  Base address: %p\n", block->block);
    printf("  Element 0: %p\n", block->block);
    printf("  Element 1: %p\n", (char*)block->block + block->item_size);
    printf("  Element 2: %p\n", (char*)block->block + 2 * block->item_size);
    
    // Free memory
    free_memory_block(block);
    printf("\n✅ Memory freed successfully\n");
    
    return 0;
}