#include <string.h>
#include <kernel/sys/kheap.h>

/**
 * @brief Выделение памяти при использовании кучи вместо стека
 *
 * @param nmemb - ???
 * @param size  - ???
 * @return void* - ???
 */
void* calloc(size_t nmemb, size_t size) {
    void* ptr = (void*)kmalloc(nmemb * size); // Explicitly cast to void*
    if (!ptr) {
        return NULL;
    }
    memset(ptr, 0, nmemb * size);
    return ptr;
}
