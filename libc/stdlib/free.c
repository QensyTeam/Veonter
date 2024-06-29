#include <kernel/kernel.h>

void free(void* ptr)
{
    return kfree(ptr);
}