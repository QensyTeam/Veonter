#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>
#include "../multiboot.h"

typedef struct header {
    size_t size;
    unsigned int magic;
    struct header* next;
} header_t;

typedef struct {
    size_t size;
    unsigned int magic;
} footer_t;

void kheap_init(void* start, size_t size);
void* kmalloc(size_t size);
void kfree(void* ptr);
size_t calculate_heap_size(multiboot_info_t* multiboot_info);

#endif
