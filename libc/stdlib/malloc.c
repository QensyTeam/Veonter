#include <kernel/kernel.h>

void* malloc(size_t size) {
    return kmalloc(size);
}
