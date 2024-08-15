#include <stddef.h>   // для size_t
#include <stdint.h>   // для uint8_t
#include <stdlib.h>   // для malloc
#include <string.h>

void* calloc(size_t num, size_t size) {
    size_t total_size = num * size;  // общий размер памяти для выделения
    void* ptr = malloc(total_size);  // выделяем память с помощью malloc

    if (ptr != NULL) {
        // очищаем выделенную память нулевыми байтами
        /*uint8_t* byte_ptr = (uint8_t*)ptr;
        for (size_t i = 0; i < total_size; ++i) {
            byte_ptr[i] = 0;
        }*/
        memset(ptr, 0, total_size);
    }

    return ptr;  // возвращаем указатель на выделенную и очищенную память
}
