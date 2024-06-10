#include <string.h>

void* memset(void* bufptr, int value, size_t size) {
    unsigned char* buf = (unsigned char*) bufptr;
    unsigned char val = (unsigned char) value; // Приводим к беззнаковому типу
    for (size_t i = 0; i < size; i++)
        buf[i] = val;
    return bufptr;
}
