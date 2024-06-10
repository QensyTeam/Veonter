#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>
#include "../multiboot.h"

void kheap_init(void* start, size_t size);
void* kmalloc(size_t size);
void kfree(void* ptr);
size_t calculate_heap_size(multiboot_info_t* multiboot_info);

#endif
