#include <kernel/kernel.h>
#include <stdint.h>
#include <stdio.h>

#define HEAP_MAGIC 0x12345678
#define HEAP_MIN_SIZE 0x70000

typedef struct header {
    size_t size;
    unsigned int magic;
    struct header* next;
} header_t;

typedef struct {
    size_t size;
    unsigned int magic;
} footer_t;

static void* heap_start;
static size_t heap_size;
static header_t* free_list;

void kheap_init(void* start, size_t size) {
    heap_start = start;
    heap_size = size;
    free_list = (header_t*)heap_start;

    // Initial block
    free_list->size = heap_size - sizeof(header_t) - sizeof(footer_t);
    free_list->magic = HEAP_MAGIC;
    free_list->next = NULL;

    footer_t* initial_footer = (footer_t*)((char*)heap_start + heap_size - sizeof(footer_t));
    initial_footer->size = free_list->size;
    initial_footer->magic = HEAP_MAGIC;
    check();
    printf("KHEAP initialization completed successfully!\n");
}

void* kmalloc(size_t size) {
    if (size == 0) return NULL;

    // Find a free block
    header_t* current = free_list;
    while (current) {
        if (current->magic != HEAP_MAGIC) {
            panic("Heap corruption detected!", "kheap.c", 42);
            return NULL;
        }

        if (current->size >= size) {
            // Allocate from this block
            if (current->size > size + sizeof(header_t) + sizeof(footer_t)) {
                // Split the block
                header_t* new_block = (header_t*)((char*)current + sizeof(header_t) + size);
                new_block->size = current->size - size - sizeof(header_t) - sizeof(footer_t);
                new_block->magic = HEAP_MAGIC;
                new_block->next = current->next;

                footer_t* new_footer = (footer_t*)((char*)new_block + new_block->size + sizeof(header_t));
                new_footer->size = new_block->size;
                new_footer->magic = HEAP_MAGIC;

                current->size = size;
                current->next = new_block;
            }

            // Remove current from free list
            if (current == free_list) {
                free_list = current->next;
            } else {
                header_t* prev = free_list;
                while (prev->next != current) {
                    prev = prev->next;
                }
                prev->next = current->next;
            }

            return (char*)current + sizeof(header_t);
        }

        current = current->next;
    }

    return NULL; // Out of memory
}

void kfree(void* ptr) {
    if (ptr == NULL) return;

    header_t* header = (header_t*)((char*)ptr - sizeof(header_t));
    footer_t* footer = (footer_t*)((char*)ptr + header->size);

    if (header->magic != HEAP_MAGIC || footer->magic != HEAP_MAGIC) {
        panic("Heap corruption detected!", "kheap.c", 92);
    }

    // Add the block back to the free list
    header->next = free_list;
    free_list = header;
}
