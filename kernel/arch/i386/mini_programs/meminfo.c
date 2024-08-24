#include <stdint.h>
#include <stdio.h>
#include <kernel/sys/kheap.h>

extern size_t heap_used;
extern size_t heap_size;

void meminfo_program() {
    printf("\nRAM:\n");
    printf("Used %ld B / %ld B\n", heap_used, heap_size);
    printf("Used %ld KB / %ld KB\n", heap_used/1024, heap_size/1024);
    printf("Used %ld MB / %ld MB\n", heap_used/1024/1024, heap_size/1024/1024);
    printf("\n");
}
