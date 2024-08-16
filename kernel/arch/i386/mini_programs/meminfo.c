#include <stdint.h>
#include <stdio.h>
#include <kernel/sys/kheap.h>

extern size_t heap_used;
extern size_t heap_size;

void meminfo_program() {
    printf("Used %zu / %zu bytes\n", heap_used, heap_size);
}
