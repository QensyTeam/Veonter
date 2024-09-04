#include <kernel/kernel.h>
#include <kernel/sys/kheap.h>

// Функция my_realloc
void* realloc(void* ptr, size_t new_size) {
    header_t* header = (header_t*)((char*)ptr - sizeof(header_t));
    size_t old_size = header->size;

    // Если новый размер равен 0, то поведение функции должно быть аналогично free
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }

    // Если указатель равен NULL, то поведение функции должно быть аналогично malloc
    if (ptr == NULL) {
        return malloc(new_size);
    }

    // Выделение новой области памяти
    void* new_ptr = malloc(new_size);
    if (new_ptr == NULL) {
        // Если выделение памяти не удалось, возвращаем NULL
        return NULL;
    }

    // Копирование данных из старой области памяти в новую
    memcpy(new_ptr, ptr, old_size < new_size ? old_size : new_size);

    // Освобождение старой области памяти
    free(ptr);

    return new_ptr;
}
